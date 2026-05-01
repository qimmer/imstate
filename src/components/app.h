#pragma once

#include <raylib.h>
#include <stdint.h>

typedef int(*GameScreen)(void);

typedef struct {
  bool loaded;
  GameScreen currentScreen;
  Font font;
  Music music;
  Sound fxCoin;
} AppState;

static AppState defaultAppState = {
  .loaded = false,
  .currentScreen = 0,
  .font = { 0 },
  .fxCoin = { 0 },
  .music = { 0 }
};

static const char * rootStateKey = "root";

void BeginApp(uint64_t id, bool *shouldClose);
void EndApp(uint64_t id);
