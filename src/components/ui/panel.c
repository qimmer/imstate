#include <raylib.h>
#include "panel.h"
#include "components/ui/clipping.h"
#include "shadow.h"
#include "utils/state.h"

void BeginPanel(uint64_t id, PanelConfig config) {
  BeginFlex(id, config.flex);
    Rectangle rect = GetFlexRect();
    Vector2 mouse = GetMousePosition();
    bool hovered = !IsPointClipped(mouse);
    bool pressed = hovered && IsMouseButtonDown(0);

    BeginReactive(Pressed, bool, pressed);
      if(pressed) {
        if(config.OnMouseDown) {
          config.OnMouseDown(config.context);
        }
      } else {
        if(config.OnMouseUp) {
          config.OnMouseUp(config.context);
        }

        if(hovered && config.OnClick) {
          config.OnClick(config.context);
        }
      }
    EndReactive(Pressed);

    Color hoverColor = config.hoverColor.a > 0 ? config.hoverColor : config.color;
    Color pressedColor = config.pressedColor.a > 0 ? config.pressedColor : hoverColor;
    Color color = pressed ? pressedColor : hovered ? hoverColor : config.color;

    if(config.shadowColor.a > 0) {
      DrawRoundedShadow(rect, config.shadowColor, fmaxf(config.rounding, config.shadow), config.shadow, 0.5, 2, 2);
    }
    
    if(config.rounding > 0) {
      DrawRoundedRect(rect, color, config.rounding);

      if(config.borderColor.a > 0) {
        DrawRoundedRectLines(rect, config.borderColor, config.rounding);
      }
    } else {
      DrawRectangle(rect.x, rect.y, rect.width, rect.height, color);

      if(config.borderColor.a > 0) {
        DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, config.borderColor);
      }
    }
}

void EndPanel(uint64_t id) {    
  EndFlex(id);
}

