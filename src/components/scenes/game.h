#pragma once

#include <raylib.h>
#include <stdint.h>

typedef struct {
  RenderTexture2D target;
} GameConfig;

void BeginGame(uint64_t id, GameConfig config);
void EndGame(uint64_t id);