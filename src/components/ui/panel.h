#pragma once

#include "flex.h"

typedef void(*OnPanelHandler)(void *context);

typedef struct {
  FlexConfig flex;
  Color color, shadowColor, borderColor, hoverColor, pressedColor;
  int rounding;
  int shadow;

  void *context;

  OnPanelHandler OnClick, OnMouseDown, OnMouseUp;
} PanelConfig;

void BeginPanel(uint64_t id, PanelConfig config);
void EndPanel(uint64_t id);