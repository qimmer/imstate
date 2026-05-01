#include <math.h>
#include <raylib.h>

inline static void DrawRoundedRect(Rectangle rect, Color color, int radius) {
  float roundedness = radius / fmin(rect.width, rect.height);

  DrawRectangleRounded(rect, roundedness, radius, color);
}

inline static void DrawRoundedRectLines(Rectangle rect, Color color, int radius) {
  float roundedness = radius / fmin(rect.width, rect.height);

  DrawRectangleRoundedLines(rect, roundedness, radius, color);
}

inline static void DrawRoundedShadow(Rectangle rect, Color color, int radius, int width, float alpha, int offsetX, int offsetY) {
  int layers = width / 2;

  float roundedness = radius / fmin(rect.width, rect.height);

  for (int i = 0; i < layers; i++)
  {
    float t = ((float)i / layers);
    float spread = t * width;
        float ringAlpha = 0.15f * (1.0f - (float)i / layers) * alpha;

    Rectangle r = {
        rect.x + offsetX - spread * 0.5f,
        rect.y + offsetY - spread * 0.5f,
        rect.width + spread,
        rect.height + spread
    };

    DrawRoundedRect(r, Fade(color, ringAlpha), radius + (t * width));
  }
}

