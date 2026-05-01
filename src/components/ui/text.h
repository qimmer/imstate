#pragma once

#include "components/ui/flex.h"
#include <stdint.h>

typedef struct {
  int themeFont;
  int themeFontSize;
  int themeColor;
  FlexConfig flex;
} TextConfig;

Rectangle _Text(uint64_t id, const char *contents, TextConfig config);
Rectangle _Icon(uint64_t id, int codepoint, TextConfig config);

#define Text(id, contents, ...) _Text(id, contents, (TextConfig)__VA_ARGS__)
#define Icon(id, icon, ...) _Icon(id, icon, (TextConfig)__VA_ARGS__)