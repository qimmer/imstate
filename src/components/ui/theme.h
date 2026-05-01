#pragma once

#include <raylib.h>
#include "utils/state.h"

enum {
  THEME_FONT_DEFAULT,
  THEME_FONT_MONO,
  THEME_FONT_ICON,

  THEME_FONT_COUNT
};

enum {
  THEME_FONTSIZE_M,
  
  THEME_FONTSIZE_2XS,
  THEME_FONTSIZE_XS,
  THEME_FONTSIZE_S,
  THEME_FONTSIZE_L,
  THEME_FONTSIZE_XL,
  THEME_FONTSIZE_2XL,

  THEME_FONTSIZE_COUNT
};

enum {
  THEME_SPACING_M,

  THEME_SPACING_3XS,
  THEME_SPACING_2XS,
  THEME_SPACING_XS,
  THEME_SPACING_S,
  THEME_SPACING_L,
  THEME_SPACING_XL,
  THEME_SPACING_2XL,

  THEME_SPACING_COUNT
};

enum {
  THEME_SIZE_M,

  THEME_SIZE_2XS,
  THEME_SIZE_XS,
  THEME_SIZE_S,
  THEME_SIZE_L,
  THEME_SIZE_XL,
  THEME_SIZE_2XL,

  THEME_SIZE_COUNT
};

enum {
  THEME_ROUNDING_M,

  THEME_ROUNDING_NONE,
  THEME_ROUNDING_S,
  THEME_ROUNDING_L,
  THEME_ROUNDING_FULL,

  THEME_ROUNDING_COUNT
};

enum {
  THEME_COLOR_FOREGROUND,
  THEME_COLOR_FOREGROUND_INPUT,
  THEME_COLOR_FOREGROUND_DISABLED,

  THEME_COLOR_BACKGROUND,
  THEME_COLOR_BACKGROUND_HOVER,
  THEME_COLOR_BACKGROUND_PRESSED,
  THEME_COLOR_BACKGROUND_INPUT,
  THEME_COLOR_BACKGROUND_DISABLED,

  THEME_COLOR_BORDER,
  THEME_COLOR_SHADOW,
  THEME_COLOR_ACCENT,
  THEME_COLOR_ACCENT_FOREGROUND,

  THEME_COLOR_DANGER,
  THEME_COLOR_WARNING,
  THEME_COLOR_INFO,
  THEME_COLOR_SUCCESS,
  THEME_COLOR_SECONDARY,

  THEME_COLOR_COUNT
};

typedef struct {
  const char* filePath;
  float size;
  int *codePoints;
  int codePointsCount;
} ThemeFont;

typedef struct {
  Color colors[THEME_COLOR_COUNT];
  ThemeFont fonts[THEME_FONT_COUNT];
  float fontSizes[THEME_FONTSIZE_COUNT];
  int roundings[THEME_ROUNDING_COUNT];
  int spacings[THEME_SPACING_COUNT];
  int sizes[THEME_SIZE_COUNT];
} Theme;

void BeginTheme(Theme initialTheme);
void EndTheme();

#define UseTheme(state) UseContext(Theme, state)

typedef struct {
  float hueDegrees;
  float saturation;
  bool dark;
  float scale;
  ThemeFont fonts[THEME_FONT_COUNT];
} DefaultThemeConfig;

static inline Theme getDefaultTheme(DefaultThemeConfig config) {
  float spacing = 32 * config.scale;
  float rounding = 16 * config.scale;
  float sizing = 28 * config.scale;
  
  return (Theme) {
    .colors[THEME_COLOR_BACKGROUND] = ColorFromHSV(config.hueDegrees, 0.1 * config.saturation, config.dark ? 0.2 : 0.9),
    .colors[THEME_COLOR_BACKGROUND_INPUT] = ColorFromHSV(config.hueDegrees, 0.1 * config.saturation, config.dark ? 0.1 : 1.0),
    .colors[THEME_COLOR_BACKGROUND_HOVER] = ColorFromHSV(config.hueDegrees, 0.1 * config.saturation, config.dark ? 0.15 : 0.95),
    .colors[THEME_COLOR_BACKGROUND_PRESSED] = ColorFromHSV(config.hueDegrees, 0.1 * config.saturation, config.dark ? 0.05 : 0.8),
    .colors[THEME_COLOR_BACKGROUND_DISABLED] = ColorFromHSV(config.hueDegrees, 0.1 * config.saturation, config.dark ? 0.4 : 0.9),
    .colors[THEME_COLOR_FOREGROUND] = ColorFromHSV(0, 0, config.dark ? 0.9 : 0.1),
    .colors[THEME_COLOR_FOREGROUND_INPUT] = ColorFromHSV(0, 0, config.dark ? 1.0 : 0.0),
    .colors[THEME_COLOR_FOREGROUND_DISABLED] = ColorFromHSV(0, 0, config.dark ? 0.8 : 0.3),
    .colors[THEME_COLOR_BORDER] = ColorFromHSV(0, 0, config.dark ? 0.6 : 0.4),
    .colors[THEME_COLOR_SHADOW] = ColorFromHSV(0, 0, 0.0),
    .colors[THEME_COLOR_ACCENT] = ColorFromHSV(config.hueDegrees, config.saturation, 0.5),
    .colors[THEME_COLOR_ACCENT_FOREGROUND] = ColorFromHSV(0, 0, 1.0),
    .colors[THEME_COLOR_DANGER] = ColorFromHSV(0, config.saturation, 0.5),
    .colors[THEME_COLOR_WARNING] = ColorFromHSV(90, config.saturation, 0.5),
    .colors[THEME_COLOR_INFO] = ColorFromHSV(270, config.saturation, 1.0),
    .colors[THEME_COLOR_SUCCESS] = ColorFromHSV(180, config.saturation, 0.5),
    .colors[THEME_COLOR_SECONDARY] = ColorFromHSV(0, 0, config.dark ? 0.6 : 0.5),

    .spacings[THEME_SPACING_3XS] = spacing / 16,
    .spacings[THEME_SPACING_2XS] = spacing / 8,
    .spacings[THEME_SPACING_XS] = spacing / 4,
    .spacings[THEME_SPACING_S] = spacing / 2,
    .spacings[THEME_SPACING_M] = spacing,
    .spacings[THEME_SPACING_L] = spacing * 2,
    .spacings[THEME_SPACING_XL] = spacing * 4,
    .spacings[THEME_SPACING_2XL] = spacing * 8,

    .roundings[THEME_ROUNDING_NONE] = 0,
    .roundings[THEME_ROUNDING_S] = rounding / 2,
    .roundings[THEME_ROUNDING_M] = rounding,
    .roundings[THEME_ROUNDING_L] = rounding * 2,
    .roundings[THEME_ROUNDING_FULL] = 999999,

    .sizes[THEME_SIZE_2XS] = sizing / 8,
    .sizes[THEME_SIZE_XS] = sizing / 4,
    .sizes[THEME_SIZE_S] = sizing / 2,
    .sizes[THEME_SIZE_M] = sizing,
    .sizes[THEME_SIZE_L] = sizing * 2,
    .sizes[THEME_SIZE_XL] = sizing * 4,
    .sizes[THEME_SIZE_2XL] = sizing * 8,

    .fontSizes[THEME_FONTSIZE_XS] = config.scale * 0.6,
    .fontSizes[THEME_FONTSIZE_S] = config.scale * 0.8,
    .fontSizes[THEME_FONTSIZE_M] = config.scale,
    .fontSizes[THEME_FONTSIZE_L] = config.scale * 1.25,
    .fontSizes[THEME_FONTSIZE_XL] = config.scale * 2.5,
    .fontSizes[THEME_FONTSIZE_2XL] = config.scale * 4,

    .fonts[THEME_FONT_DEFAULT] = config.fonts[THEME_FONT_DEFAULT],
    .fonts[THEME_FONT_MONO] = config.fonts[THEME_FONT_MONO],
    .fonts[THEME_FONT_ICON] = config.fonts[THEME_FONT_ICON],
  };
}

