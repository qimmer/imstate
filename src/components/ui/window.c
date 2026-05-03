#include "window.h"
#include <raylib.h>
#include "components/ui/flex.h"
#include "utils/flex.h"
#include "window.h"
#include "panel.h"
#include "theme.h"

void BeginWindow(uint64_t id, WindowConfig config) {
  UseTheme(theme);
  MakeId(TitleBar);
  MakeId(Content);
  MakeId(ContentContainer);

  FlexConfig containerFlexConfig = config.flex;
  
  int padding = config.noPadding ? 0 : theme->spacings[THEME_SPACING_S];

  BeginPanel(id, (PanelConfig){
    .color = theme->colors[THEME_COLOR_BACKGROUND],
    .shadowColor = theme->colors[THEME_COLOR_SHADOW],
    .flex = {
      .direction = FlexVertical,
      .align = FlexStretch,
      .w = config.flex.w,
      .h = config.flex.h,
    },
    .shadow = 16,
  });
    if(config.title && config.title[0]) {
      BeginFlex(TitleBar, (FlexConfig) {
        .direction = FlexHorizontal,
        .align = FlexCenter,
        .pl = padding,
        .pr = padding,
        .pt = padding,
        .pb = padding,
      }); 

      EndFlex(TitleBar);
    }
    
    BeginFlex(ContentContainer, (FlexConfig) {
      .direction = FlexVertical,
      .align = FlexStretch,
      .justify = FlexSpaceBetween,
      .pl = padding,
      .pr = padding,
      .pt = padding,
      .pb = padding,
    });
      BeginFlex(Content, containerFlexConfig);
}

void EndWindow(uint64_t id) {   
  MakeId(Content);
  MakeId(ContentContainer);

      EndFlex(Content);
    EndFlex(ContentContainer);
  EndPanel(id);
}