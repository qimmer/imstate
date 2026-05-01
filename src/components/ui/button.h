#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef void(*OnButtonHandler)(struct ButtonConfig *config);

typedef enum {
  BUTTON_DEFAULT,
  BUTTON_PRIMARY,
  BUTTON_SECONDARY,
  BUTTON_FLAT,
  BUTTON_ICON,

  BUTTON_COUNT
} ButtonVariant;

typedef struct {
  ButtonVariant variant;
  const char *title;
  bool disabled;
  
  void *context;

  OnButtonHandler OnClick;
} ButtonConfig;

void Button(uint64_t id, ButtonConfig config);