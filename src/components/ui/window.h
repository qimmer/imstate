#pragma once

#include "flex.h"

typedef struct {
  FlexConfig flex;
  const char *title;
  bool noPadding;
} WindowConfig;

void BeginWindow(uint64_t id, WindowConfig config);
void EndWindow(uint64_t id);