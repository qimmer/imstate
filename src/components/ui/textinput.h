#pragma once

#include "components/ui/flex.h"
#include "components/ui/text.h"
#include <stdint.h>

typedef void(*OnTextInputChangeHandler)(void *context, const char *newValue);

typedef struct {
  TextConfig textConfig;
  FlexConfig flex;
  const char *initialValue;
  bool multiline;
  void *context;

  OnTextInputChangeHandler OnValueChange;
} TextInputConfig;

void TextInput(uint64_t id, TextInputConfig config);
