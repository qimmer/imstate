#pragma once

#include "components/ui/icons.h"
#include <stdbool.h>
#include <stdint.h>

struct TreeConfig;

typedef void(*OnTreeHandler)(uint64_t id, struct TreeConfig* context);

typedef struct {
  const char *title;
  void *context;

  OnTreeHandler OnClick;
  OnTreeHandler OnExpandedChange;

  IconId Icon, ExpandedIcon;
} TreeConfig;

bool BeginTree(uint64_t id, TreeConfig config);
void EndTree(uint64_t id);
