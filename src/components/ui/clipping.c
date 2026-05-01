#include "clipping.h"
#include "utils/math.h"
#include "utils/state.h"

#include <float.h>
#include <raylib.h>
#include <stb_ds.h>

typedef struct {
  Rectangle *clipStack;
} ClipContext;

bool RectOverlap(Rectangle a, Rectangle b)
{
    return !(a.x + a.width  <= b.x ||  // a is left of b
             b.x + b.width  <= a.x ||  // b is left of a
             a.y + a.height <= b.y ||  // a is above b
             b.y + b.height <= a.y);   // b is above a
}

bool PointInRect(Vector2 p, Rectangle r)
{
    return (p.x >= r.x &&
            p.x <  r.x + r.width &&
            p.y >= r.y &&
            p.y <  r.y + r.height);
}

void BeginClipRoot() {
  BeginContext(ClipContext, clipping, {});
    BeginClip((Rectangle) {
      .x = 0,
      .y = 0,
      .width = GetScreenWidth(),
      .height = GetScreenHeight()
    });
}

void EndClipRoot() {
    EndClip();
  EndContext(ClipContext, clipping);
}

Rectangle ClipRectangle(Rectangle parent, Rectangle child) {
  Vector2 parentStart = (Vector2){ .x = parent.x, .y = parent.y };
  Vector2 parentEnd = (Vector2){ .x = parent.x + parent.width, .y = parent.y + parent.height };
  Vector2 childStart = (Vector2){ .x = child.x, .y = child.y };
  Vector2 childEnd = (Vector2){ .x = child.x + child.width, .y = child.y + child.height };

  Vector2 newStart = (Vector2){ .x = MAX(parentStart.x, childStart.x), .y = MAX(parentStart.y, childStart.y) };
  Vector2 newEnd = (Vector2){ .x = MIN(parentEnd.x, childEnd.x), .y = MIN(parentEnd.y, childEnd.y) };

  Rectangle newRect = {
    .x = newStart.x,
    .y = newStart.y,
    .width = newEnd.x - newStart.x,
    .height = newEnd.y - newStart.y,
  };

  return newRect;
}

Rectangle GetClipping() {
  UseContext(ClipContext, clipping);

  Rectangle parent = arrlast(clipping->clipStack);

  return parent;
}

void BeginClip(Rectangle rect) {
  UseContext(ClipContext, clipping);

  if(arrlen(clipping->clipStack) <= 0) {
    arrpush(clipping->clipStack, rect);
    return;
  }

  Rectangle parent = GetClipping();
  Rectangle clipped = ClipRectangle(parent, rect);  

  arrpush(clipping->clipStack, clipped);
}

void EndClip() {
  UseContext(ClipContext, clipping);

  arrpop(clipping->clipStack);
}

IsClippedResult IsRectClipped(Rectangle rect) {
  UseContext(ClipContext, clipping);

  if(arrlen(clipping->clipStack) <= 0) {
    return false;
  }

  Rectangle parent = GetClipping();

  return !RectOverlap(parent, rect);

}

bool IsPointClipped(Vector2 p) {
  UseContext(ClipContext, clipping);

  if(arrlen(clipping->clipStack) <= 0) {
    return false;
  }

  Rectangle parent = GetClipping();

  return !PointInRect(p, parent);
}

bool IsHovering() {
  return !IsPointClipped((Vector2) {.x = GetMouseX(), .y = GetMouseY()});
}