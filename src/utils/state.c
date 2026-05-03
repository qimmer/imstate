#include "state.h"
#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stb_ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>

#include "utils/math.h"

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
  arrsetcap(b->stateArenaOffsets, initialStateCount);
  b->arena_offset = 0;
}

static void BufferFree(StateBuffer *b) {
  arrfree(b->stateArenaOffsets);
  arrfree(b->arena);
}

static void BufferReset(StateBuffer* b) {
    b->arena_offset = 0;
    b->depth = 0;
    b->stateCursorIndex = 0;
    arrsetlen(b->stateArenaOffsets, 0);
}

static State* AllocState(StateBuffer* b, StateId id, size_t data_size) {
    size_t total = sizeof(State) + data_size;

    if (b->arena_offset + total > arrlen(b->arena)) {
      arrsetlen(b->arena, arrlen(b->arena) * 2);
    }

    size_t arenaOffset = b->arena_offset;
    State* s = (State*)(b->arena + arenaOffset);
    b->arena_offset += total;

    s->id = id;
    s->parentId = NoStateId;
    s->data_size = data_size;
    s->onCleanup = NULL;

    arrpush(b->stateArenaOffsets, arenaOffset);
    return s;
}

static inline StateBuffer *GetPrevBuffer() {
  return &g_context->buffers[1 - g_context->curr];
}

static inline StateBuffer *GetCurrBuffer() {
  return &g_context->buffers[g_context->curr];
}

static inline State* GetState(StateBuffer* b, StateIndex index) {
  return (State*)&b->arena[b->stateArenaOffsets[index]];
}

static inline State* FindPrevSibling(StateId parentId, StateId id, int depth, int* out_index) {
  StateBuffer *prev = GetPrevBuffer();
  int len = arrlen(prev->stateArenaOffsets);
  int i1 = MIN(prev->stateCursorIndex, len - 1); // Iterates upwards
  int i2 = prev->stateCursorIndex + 1; // Iterates downwards
  int foundIndex = -1;
  
  while(foundIndex == -1 && (i1 >= 0 || i2 < len)) {
    if(i1 >= 0) {
      State* s1 = GetState(prev, i1);
      // If we exit out of the parent descendant tree, don't expect more siblings
      if(s1->depth < depth-1) {
        i1 = -INT_MAX + 1;
      } else if(s1->parentId == parentId && s1->id == id) {
        foundIndex = i1;
        break;
      }
      --i1;
    }
    
    if(i2 < len) {
      State* s2 = GetState(prev, i2);
      // If we exit out of the parent descendant tree, don't expect more siblings
      if(s2->depth < depth-1) {
        i2 = INT_MAX - 1;
      } else if(s2->parentId == parentId && s2->id == id){
        foundIndex = i2;
        break;
      }
      ++i2;
    }
  }

  if(foundIndex >= 0) {
    assert(foundIndex >= 0 && foundIndex < arrlen(prev->stateArenaOffsets));
    prev->stateCursorIndex = foundIndex;
    return GetState(prev, foundIndex);
  }

  return NULL;
}


void CreateStateContext(StateContext *stateContext, size_t initialArenaSize, size_t initialStateCount) {
  memset(stateContext, 0, sizeof(StateContext));
  BufferInit(&stateContext->buffers[0], initialArenaSize, initialStateCount);
  BufferInit(&stateContext->buffers[1], initialArenaSize, initialStateCount);
  stateContext->curr = 0;
  stateContext->currentParentIndex = NoStateIndex;  
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
  g_context->currentParentIndex = NoStateIndex;
}

void EndFrame() {
  StateBuffer *curr = GetCurrBuffer(), *prev = GetPrevBuffer();
  assert(curr->depth == 0);
    
  // detect removals
  for (int i = 0; i < arrlen(prev->stateArenaOffsets); i++) {
    State* old = GetState(prev, i);

    int found = 0;
    for (int j = 0; j < arrlen(curr->stateArenaOffsets); j++) {
      State* cur = GetState(curr, j);
      if (cur->id == old->id && cur->parentId == old->parentId) {
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
  StateIndex parentIndex = g_context->currentParentIndex;
  
  // try fast path: same position
  State* s = AllocState(curr, id, size);
  assert(!!s);
  
  State *p = GetState(curr, parentIndex);
  State* previousState = FindPrevSibling(p->id, id, curr->depth, NULL);
  s->parentId = p->id;
  s->onCleanup = config.onCleanup;
  s->depth = curr->depth;

  // push stack
  arrpush(g_context->stack, arrlen(curr->stateArenaOffsets) - 1);
  g_context->currentParentIndex = arrlen(curr->stateArenaOffsets) - 1;
  curr->depth++;

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

  StateIndex idx = arrpop(g_context->stack);
  State* s = GetState(curr, idx);
  curr->depth--;

  if (s->id != id) {
    fprintf(stderr, "Begin/End mismatch (%lu vs %lu)\n", s->id, id);
  }

  g_context->currentParentIndex = (arrlen(g_context->stack) > 0) ? g_context->stack[arrlen(g_context->stack) - 1] : NoStateId;
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

StateId HashId(const char *s) { 
  uint64_t hash = HashFnv1a(s, strlen(s));
  StateId id = *((StateId*)&hash);
  RegisterId(id, s);
  return id;
}

void* FindNearestState(StateId id, int skip) {
  StateBuffer *curr = GetCurrBuffer();

  for (int i = arrlen(g_context->stack) - 1; i >= 0; i--) {
      int idx = g_context->stack[i];
      State* s = GetState(curr, idx);

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
      StateIndex idx = g_context->stack[i];
      State* s = GetState(curr, idx);

      printf("/%s", GetIdName(s->id));
    }
    
    printf("\n");

    abort();
  }  
}
    