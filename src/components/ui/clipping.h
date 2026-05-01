#pragma once

#include <raylib.h>

typedef enum {
  CLIPPED_NOT,
  CLIPPED_PARTLY,
  CLIPPED_ALL,

  CLIPPED_COUNT
} IsClippedResult;

bool IsPointClipped(Vector2 p);
IsClippedResult IsRectClipped(Rectangle rect);

bool IsHovering();

void BeginClipRoot();
void EndClipRoot();

void BeginClip(Rectangle rect);
void EndClip();

