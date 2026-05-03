#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

typedef void (*OnCleanupCallback)(void *state);
typedef void (*OnInitCallback)(void *state, const void *config);

typedef uint64_t StateId;
typedef int StateIndex;
#define NoStateId UINT64_MAX
#define NoStateIndex (-1)

typedef struct {
  int idx;
  StateId id;
} StateStackEntry;

typedef struct State {
    StateId id;
    int depth;

    StateId parentId;

    size_t data_size;
    OnCleanupCallback onCleanup;

    unsigned char data[];
} State;

typedef State* StatePtr;

typedef struct {
    unsigned char* arena;
    size_t arena_offset;
    int depth;
    int stateCursorIndex;

    size_t* stateArenaOffsets;
} StateBuffer;

typedef struct {
  OnInitCallback onInit;
  OnCleanupCallback onCleanup;
  const void * defaultState;
  void *config;
} StateConfig;

typedef struct {
  StateId key;
  const char * value;
} StateIdRegistrationEntry;

typedef struct {
  StateId id;
  const char *name;
  StatePtr* stack;
} ContextInfo;

typedef struct {
  StateBuffer buffers[2];
  int curr;
  StateIndex *stack;
  StateIndex currentParentIndex;
  StateStackEntry **prevChildrenStack;
  StateIdRegistrationEntry *idNameMap;
  ContextInfo *contextInfos;
} StateContext;

void CreateStateContext(StateContext *stateContext, size_t initialArenaSize, size_t initialStateCount);
void DestroyStateContext(StateContext *stateContext);
void SetStateContext(StateContext *stateContext);
StateContext *GetStateContext();

int GetContextIndex(StateId contextId);
void* GetContextFromIndex(int contextIndex, int skip);
void PopContext(int index);
void PushContext(int index, StatePtr state);

void BeginFrame();
void EndFrame();
bool _BeginState(StateId id, void **data, size_t size, StateConfig config);
void EndState(StateId id);
void* FindNearestState(StateId id, int skip);
StateId HashId(const char *s);
void RegisterId(StateId id, const char *s);
const char * GetIdName(StateId id);

#define MakeId(id) \
  static StateId id;\
  if(!id) {\
    id = HashId(#id);\
    RegisterId(id, #id);\
  }

#define BeginState(id, stateType, state, ...) \
  stateType *state = 0;\
  bool _init##id = _BeginState(id, (void**)&state, sizeof(stateType), (StateConfig)__VA_ARGS__)


#define BeginContext(stateType, state, ...) \
  static StateId state##id;\
  static int state##idx;\
  if(!state##id) {\
    state##id = HashId(#stateType);\
    state##idx = GetContextIndex(state##id);\
  }\
  stateType *state = 0;\
  bool _init##stateType = _BeginState(state##id, (void**)&state, sizeof(stateType), (StateConfig)__VA_ARGS__);\
  PushContext(state##idx, (void*)state)

#define EndContext(stateType, state) \
  static StateId state##id2;\
  static int state##idx2;\
  if(!state##id2) {\
    state##id2 = HashId(#stateType);\
    state##idx2 = GetContextIndex(state##id2);\
  }\
  PopContext(state##idx2);\
  EndState(state##id2)


#define UseContext(type, name)\
  static StateId name##ctx;\
  static int name##idx;\
  if(!name##ctx) {\
    name##ctx = HashId(#type);\
    name##idx = GetContextIndex(name##ctx);\
  }\
  type *name = (type*)GetContextFromIndex(name##idx, 0);

#define UseContextSkip(type, name, skip)\
  static StateId name##ctx;\
  static int name##idx;\
  if(!name##ctx) {\
    name##ctx = HashId(#type);\
    name##idx = GetContextIndex(name##ctx);\
  }\
  type *name = (type*)GetContextFromIndex(name##idx, skip);

#define BeginInit(id) \
  if(_init##id) {

#define EndInit() \
  }

#define BeginFor(id, i, len) \
  BeginId(id);\
  for(StateId i = 0; i < len; ++i) {\
    BeginId(i)

#define EndFor(id, i)\
    EndId(i);\
  } EndId(id)

#define BeginId(id) \
  _BeginState(id, NULL, 0, (StateConfig){})

#define EndId(id) EndState(id)

#define BeginReactive(id, variableType, ...) \
  MakeId(Reactive##id);\
  variableType *_last##id;\
  size_t _sizeOf##id = sizeof(variableType);\
  variableType _current##id = __VA_ARGS__;\
  _BeginState(Reactive##id, (void**)&_last##id, sizeof(variableType), (StateConfig) { .defaultState = &(_current##id) });\
  if(memcmp(_last##id, &_current##id, sizeof(variableType)) != 0) {\
    memcpy(_last##id, &_current##id, _sizeOf##id);

#define EndReactive(id)\
  }\
  EndState(Reactive##id)\

void AssertContext(void *ctx);