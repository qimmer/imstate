
#include "utils/state.h"

#include <stb_ds.h>

typedef struct {
  StateId currentFocusId;
} FocusState;

typedef struct {
  StateId currentFocusId;
  StateId *focusableIdStack;
} FocusContext;

void BeginFocusRoot() {
  BeginContext(FocusContext, focusContext, {});
}
void EndFocusRoot() {
  EndContext(FocusContext, focusContext);
}

void BeginFocusable(StateId id) {
  UseContext(FocusContext, focusContext);

  arrpush(focusContext->focusableIdStack, id);
}

void EndFocusable(StateId id) {
  UseContext(FocusContext, focusContext);

  arrpop(focusContext->focusableIdStack);

}

void Focus() {
  UseContext(FocusContext, focusContext);

  focusContext->currentFocusId = arrlen(focusContext->focusableIdStack) > 0 ? arrlast(focusContext->focusableIdStack) : -1;
}
void Blur() {
  UseContext(FocusContext, focusContext);
  focusContext->currentFocusId = -1;
}

bool IsFocused() {
  UseContext(FocusContext, focusContext);

  return focusContext->currentFocusId != -1 && arrlast(focusContext->focusableIdStack) == focusContext->currentFocusId;
}

void BeginFocus() {
  BeginContext(FocusState, state, {});
}

void EndFocus() {
  EndContext(FocusState, state);
}

void SetFocus(StateId id) {
  UseContext(FocusState, state);

  state->currentFocusId = id;
}

StateId GetFocus() {
  UseContext(FocusState, state);

  return state->currentFocusId;
}

bool HasFocus() {
  UseContext(FocusState, state);

  return state->currentFocusId;
}
