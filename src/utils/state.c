#include "state.h"
#include <assert.h>
#include <stdbool.h>
#include <stb_ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

thread_local StateContext* g_context = NULL;

void RegisterId(StateId id, const char *name) {
  StateIdRegistrationEntry e = {
    .key = id,
    .value = name
  };
  hmputs(g_context->idNameMap, e);
}

bool HasIdName(StateId id) {
  StateIdRegistrationEntry *e = hmgetp_null(g_context->idNameMap, id);
  return !!e;
}

const char * GetIdName(StateId id) {
  StateIdRegistrationEntry *e = hmgetp_null(g_context->idNameMap, id);
  if(!e) {
    char numberString[128];
    snprintf(numberString, 128, "%lu", id);
    char *allocation = malloc(strlen(numberString) + 1);
    strcpy(allocation, numberString);
    
    RegisterId(id, allocation);
    e = hmgetp(g_context->idNameMap, id);
  }

  return e->value;
}

static void BufferInit(StateBuffer* b, size_t initialArenaSize, size_t initialStateCount) {
  arrsetlen(b->arena, initialArenaSize);
  arrsetcap(b->states, initialStateCount);
  b->arena_offset = 0;
}

static void BufferFree(StateBuffer *b) {
  arrfree(b->states);
  arrfree(b->arena);
}

static void BufferReset(StateBuffer* b) {
    b->arena_offset = 0;

    arrsetlen(b->states, 0);
}

static State* AllocState(StateBuffer* b, StateId id, size_t data_size) {
    size_t total = sizeof(State) + data_size;

    if (b->arena_offset + total > arrlen(b->arena)) {
      arrsetlen(b->arena, arrlen(b->arena) * 2);
    }

    State* s = (State*)(b->arena + b->arena_offset);
    b->arena_offset += total;

    s->id = id;
    s->parent = NoStateIndex;
    s->first_child = NoStateId;
    s->child_count = 0;
    s->data_size = data_size;
    s->onCleanup = NULL;

    arrpush(b->states, s);
    return s;
}

static inline StateBuffer *GetPrevBuffer() {
  return &g_context->buffers[1 - g_context->curr];
}

static inline StateBuffer *GetCurrBuffer() {
  return &g_context->buffers[g_context->curr];
}

static State* FindPrevSibling(int parent, StateId id, int* out_index) {
  StateBuffer *prev = GetPrevBuffer();
  for (int i = 0; i < arrlen(prev->states); i++) {
      State* s = prev->states[i];
      if (s->parent == parent && s->id == id) {
          if (out_index) *out_index = i;
          return s;
      }
  }
  return NULL;
}


void CreateStateContext(StateContext *stateContext, size_t initialArenaSize, size_t initialStateCount) {
  memset(stateContext, 0, sizeof(StateContext));
  BufferInit(&stateContext->buffers[0], initialArenaSize, initialStateCount);
  BufferInit(&stateContext->buffers[1], initialArenaSize, initialStateCount);
  stateContext->curr = 0;
  stateContext->current_parent = NoStateId;  
}

void DestroyStateContext(StateContext *stateContext) {
  BufferFree(&stateContext->buffers[0]);
  BufferFree(&stateContext->buffers[1]);
  hmfree(stateContext->idNameMap);
}

void SetStateContext(StateContext *stateContext) {
  g_context = stateContext;
}

StateContext *GetStateContext() {
  return g_context;
}

void BeginFrame() {
  g_context->curr = 1 - g_context->curr;

  BufferReset(GetCurrBuffer());

  arrsetlen(g_context->stack, 0);
  g_context->current_parent = NoStateId;
}

void EndFrame() {
  StateBuffer *curr = GetCurrBuffer(), *prev = GetPrevBuffer();
    
  // detect removals
  for (int i = 0; i < arrlen(prev->states); i++) {
    State* old = prev->states[i];

    int found = 0;
    for (int j = 0; j < arrlen(curr->states); j++) {
      State* cur = curr->states[j];
      if (cur->id == old->id && cur->parent == old->parent) {
        found = 1;
        break;
      }
    }

    if (!found && old->onCleanup) {
      old->onCleanup(old->data);
    }
  }
}

bool _BeginState(StateId id, void **data, size_t size, StateConfig config) {
  StateBuffer *curr = GetCurrBuffer(), *prev = GetPrevBuffer();
  int parent = g_context->current_parent;

  // try fast path: same position
  State* s = AllocState(curr, id, size);
  assert(!!s);
  
  State* previousState = FindPrevSibling(parent, id, NULL);
  s->parent = parent;
  s->onCleanup = config.onCleanup;

  // attach to parent
  if (parent != NoStateIndex) {
      State* p = curr->states[parent];
      if (p->child_count == 0) {
          p->first_child = arrlen(curr->states) - 1;
      }
      p->child_count++;
  }
  
  // push stack
  arrpush(g_context->stack, arrlen(curr->states) - 1);
  g_context->current_parent = arrlen(curr->states) - 1;

  bool isInit = !previousState;
  bool isReinit = previousState && previousState->data_size != size;

  if (previousState) {
    if(!isReinit) {
      // copy old state
      memcpy(s->data, previousState->data, size < previousState->data_size ? size : previousState->data_size);
    } else {
      // Data size changed, so re-init
      if(previousState->onCleanup) previousState->onCleanup(previousState->data);
      memset(s->data, 0, size);
    }
  } else {
    memset(s->data, 0, size);
  }

  if(data) {
    *data = s->data;
  }

  if((isInit || isReinit) && config.onInit) {
    config.onInit(s->data, config.config);
  }

  return isInit || isReinit;
}

void EndState(StateId id) {
  StateBuffer *curr = GetCurrBuffer();
    if (arrlen(g_context->stack) <= 0) {
      fprintf(stderr, "Stack underflow\n");
      return;
    }

    int idx = arrpop(g_context->stack);
    State* s = curr->states[idx];

    if (s->id != id) {
      fprintf(stderr, "Begin/End mismatch (%lu vs %lu)\n", s->id, id);
    }

    g_context->current_parent = (arrlen(g_context->stack) > 0) ? g_context->stack[arrlen(g_context->stack) - 1] : NoStateId;
}

static uint64_t HashFnv1a(const void *data, size_t len) {
  uint64_t h = 1469598103934665603ULL;
  const uint8_t *p = data;

  for (size_t i = 0; i < len; i++) {
    h ^= p[i];
    h *= 1099511628211ULL;
  }

  return h;
}

uint64_t HashId(const char *s) { 
  uint64_t hash = HashFnv1a(s, strlen(s));
  RegisterId(hash, s);
  return hash;
}

void* FindNearestState(StateId id, int skip) {
  StateBuffer *curr = GetCurrBuffer();

  for (int i = arrlen(g_context->stack) - 1; i >= 0; i--) {
      int idx = g_context->stack[i];
      State* s = curr->states[idx];

      if (s->id == id) {
        if(skip <= 0) {
          return s->data;
        } else {
          skip--;
        }
      }
  }
  return NULL;
}

void AssertContext(void *ctx) {
  StateBuffer *curr = GetCurrBuffer();
  
  if(!ctx) {
    printf("Assertion failed requiring context. Current state stack:\n");

    for (int i = 0; i < arrlen(g_context->stack); ++i) {
      int idx = g_context->stack[i];
      State* s = curr->states[idx];

      printf("/%s", GetIdName(s->id));
    }
    
    printf("\n");

    abort();
  }  
}
    