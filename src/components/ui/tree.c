
#include "tree.h"
#include "components/ui/flex.h"
#include "components/ui/icons.h"
#include "components/ui/panel.h"
#include "components/ui/text.h"
#include "components/ui/theme.h"
#include "utils/flex.h"
#include "utils/state.h"

typedef struct {
  bool expanded;
} TreeState;

static void OnClick(TreeState *state) {
  state->expanded = !state->expanded;
}

bool BeginTree(uint64_t id, TreeConfig config) {
  UseContext(Theme, theme);

  MakeId(TreeRoot)
  MakeId(Panel)
  MakeId(Label)
  MakeId(ExpanderContainer)
  MakeId(ExpanderIcon)
  MakeId(Filler)
  MakeId(IconContainer)
  MakeId(Icon)

  BeginState(id, TreeState, state, {});
    BeginFlex(TreeRoot, (FlexConfig) {
      .direction = FlexVertical,
      .align = FlexStretch,
      .minh = theme->sizes[THEME_SIZE_M]
    });
      BeginPanel(Panel, (PanelConfig) {
        .OnClick = (OnPanelHandler)OnClick,
        .context = state,
        .hoverColor = theme->colors[THEME_COLOR_BACKGROUND_HOVER],
        .pressedColor = theme->colors[THEME_COLOR_BACKGROUND_PRESSED],
        .flex = {
          .direction = FlexHorizontal,
          .align = FlexCenter,
          .gap =theme->spacings[THEME_SPACING_2XS],
          .pl = theme->spacings[THEME_SPACING_2XS],
          .pr = theme->spacings[THEME_SPACING_2XS],
          .pt = theme->spacings[THEME_SPACING_2XS],
          .pb = theme->spacings[THEME_SPACING_2XS],
        } 
      });
        BeginFlex(ExpanderContainer, (FlexConfig) { .w = theme->sizes[THEME_SIZE_S] });
          Icon(ExpanderIcon, state->expanded ? ICON_CHEVRON_DOWN : ICON_CHEVRON_RIGHT, {});
        EndFlex(ExpanderContainer);

        if(config.Icon) {
          BeginFlex(IconContainer, (FlexConfig) { .w = theme->sizes[THEME_SIZE_S] });
            Icon(Icon, ICON_FOLDER, {});
          EndFlex(IconContainer);
        }

        Text(Label, config.title, {});
        Flex(Filler, (FlexConfig) { .grow = 1 });
      EndPanel(Panel);

  return state->expanded;
}

void EndTree(uint64_t id) {
  MakeId(TreeRoot)

    EndFlex(TreeRoot);
  EndState(id);
}
