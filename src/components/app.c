#include "components/ui/clipping.h"
#include "components/ui/focus.h"
#include "components/ui/flex.h"
#include "components/mainwindow.h"
#include "components/ui/icons.h"
#include "resources.h"
#include "components/ui/theme.h"
#include "scenes/editor.h"
#include "utils/state.h"

#include <raylib.h>

void BeginApp(uint64_t id, bool *shouldClose) {
  MakeId(MainWindow);

  BeginId(id);
    BeginMainWindow(MainWindow, shouldClose);
      Theme theme = getDefaultTheme((DefaultThemeConfig) {
        .dark = true,
        .hueDegrees = 320,
        .saturation = 0.7,
        .scale = 1.0,
        .fonts[THEME_FONT_DEFAULT] = { .filePath = "resources/roboto.ttf", .size = 18 },
        .fonts[THEME_FONT_MONO] = { .filePath = "resources/ProggyClean.ttf", .size = 13 },
        .fonts[THEME_FONT_ICON] = { .filePath = "resources/tabler-icons.ttf", .size = 16, .codePoints = (int*)IconCodepoints, .codePointsCount = ICON_COUNT },
      });

      BeginTheme(theme);
        BeginResources();
          BeginClipRoot();
            BeginFocusRoot();
              BeginFlexRoot((FlexRootConfig) {});
}

void EndApp(uint64_t id) {
  MakeId(MainWindow);

              EndFlexRoot();
            EndFocusRoot();
          EndClipRoot();
        EndResources();
      EndTheme();
    EndMainWindow(MainWindow);
  EndId(id);
}