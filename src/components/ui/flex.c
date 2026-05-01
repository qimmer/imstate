#include "utils/flex.h"
#include "components/ui/clipping.h"
#include "utils/state.h"
#include "flex.h"
#include <assert.h>
#include <math.h>
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
  FlexNodeRef node;
} FlexState;

static void InitChild(FlexState *state) {
  UseContextSkip(FlexState, parentState, 1);
  AssertContext(parentState);

  state->node = Flex_newNode();
  Flex_addChild(parentState->node, state->node);
}

static void InitRoot(FlexState *state) {
  state->node = Flex_newNode();
}

static void Cleanup(FlexState* state) {
  if(state->node) {
    Flex_freeNode(state->node);
  }
}

void BeginFlexRoot(FlexRootConfig config) {
  BeginContext(FlexRootConfig, flexRootConfig, {.defaultState = &config});
    BeginContext(FlexState, flexState, { .onInit = (OnInitCallback)InitRoot, .onCleanup = (OnCleanupCallback)Cleanup });
      BeginInit(FlexState)
        Flex_setAlignItems(flexState->node, FlexStretch);
        Flex_setJustifyContent(flexState->node, FlexStretch);
        Flex_setDirection(flexState->node, FlexVertical);
      EndInit()
      
      Flex_setWidth(flexState->node, GetScreenWidth());
      Flex_setHeight(flexState->node, GetScreenHeight());
}

void EndFlexRoot() {
  UseContext(FlexState, flexState);

      Flex_layout(flexState->node, GetScreenWidth(), GetScreenHeight(), 1.f);

      if(IsKeyDown(KEY_F1)) {
        printf("%s", "Flex structure:\n");
        flex_printNode(flexState->node, FlexPrintChildren, 0);
      }
    EndContext(FlexState, flexState);
  EndContext(FlexRootConfig, flexRootConfig);
}

void Flex(uint64_t id, FlexConfig config) {
  BeginFlex(id, config);
  EndFlex(id);
}

void BeginFlex(uint64_t id, FlexConfig config) {
  
  UseContext(FlexRootConfig, flexRootConfig);

  
  BeginId(id);
    BeginContext(FlexState, state, { .onInit = (OnInitCallback)InitChild, .onCleanup = (OnCleanupCallback)Cleanup});
      Flex_setAlignItems(state->node, config.align);
      Flex_setJustifyContent(state->node, config.justify);
      Flex_setDirection(state->node, config.direction);
      Flex_setSpacing(state->node, config.gap);
      Flex_setMinWidth(state->node, config.minw);
      
      if(config.w) {
        Flex_setWidth(state->node, config.w);
      } else {
        Flex_setWidthAuto(state->node);
      }

      if(config.maxw) {
        Flex_setMaxWidth(state->node, config.maxw);
      } else {
        Flex_setMaxWidth_Length(state->node, (FlexLength){ .type = FlexLengthTypeUndefined, .value = NAN });
      }

      Flex_setMinHeight(state->node, config.minh);
      if(config.h) {
        Flex_setHeight(state->node, config.h);
      } else {
        Flex_setHeightAuto(state->node);
      }

      if(config.maxh) {
        Flex_setMaxHeight(state->node, config.maxh);
      } else {
        Flex_setMaxHeight_Length(state->node, (FlexLength){ .type = FlexLengthTypeUndefined, .value = NAN });
      }
      Flex_setFlexGrow(state->node, config.grow);
      Flex_setFlexShrink(state->node, config.shrink || 1);
      Flex_setPaddingLeft(state->node, config.pl);
      Flex_setPaddingRight(state->node, config.pr);
      Flex_setPaddingTop(state->node, config.pt);
      Flex_setPaddingBottom(state->node, config.pb);
      Flex_setFixed(state->node, config.fixed);

      Rectangle rect = GetFlexRect();
      if(flexRootConfig->debugFlex) {
        DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, MAGENTA);
        //DrawText(GetIdName(id), rect.x, rect.y, 8, MAGENTA);
      }

      BeginClip(rect); 
}

void EndFlex(uint64_t id) {
      EndClip();
    EndContext(FlexState, state);
  EndId(id);

}

Rectangle GetFlexRect() {
  UseContext(FlexState, w);

  Rectangle rect = {
    .x = Flex_getResultLeft(w->node),
    .y = Flex_getResultTop(w->node),
    .width = Flex_getResultWidth(w->node),
    .height = Flex_getResultHeight(w->node)
  };

  FlexNodeRef parent = Flex_getParent(w->node);
  while(parent) {
    rect.x += Flex_getResultLeft(parent);
    rect.y += Flex_getResultTop(parent);
    parent = Flex_getParent(parent);
  }

  return rect;
}