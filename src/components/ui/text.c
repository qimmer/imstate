#include "text.h"
#include "components/ui/flex.h"
#include "components/ui/icons.h"
#include "utils/flex.h"
#include "utils/math.h"
#include "utils/state.h"
#include "components/ui/theme.h"
#include "components/resources.h"

#include <raylib.h>

Rectangle _Text(uint64_t id, const char *contents, TextConfig config) {
  UseContext(Theme, theme);
  UseContext(ResourcesState, resources);

  Font font = resources->fonts[THEME_FONTSIZE_COUNT * config.themeFont + config.themeFontSize];
  float fontSize = theme->fontSizes[config.themeFontSize] * theme->fonts[config.themeFont].size;
  float spacing = 0.0f;
  Vector2 textSize = MeasureTextEx(font, contents, fontSize, spacing);
  Color color = theme->colors[config.themeColor];

  config.flex.minw = MAX( config.flex.minw, textSize.x);
  config.flex.minh = MAX(config.flex.minh, textSize.y);

  BeginFlex(id, config.flex);    
    Rectangle rect = GetFlexRect();
    DrawTextEx(font, contents, (Vector2){ .x = rect.x , .y = rect.y  }, fontSize, spacing, color);
  EndFlex(id);

  return rect;
}

Rectangle _Icon(uint64_t id, int icon, TextConfig config) {
  char utf[65];
  int len;
  const char *tmp = CodepointToUTF8(IconCodepoints[icon], &len);
  memcpy(utf, tmp, len);
  utf[len] = '\0';

  config.themeFont = THEME_FONT_ICON;

  return _Text(id, utf, config);
}