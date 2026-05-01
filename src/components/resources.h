#pragma once

#include <raylib.h>
#include <stdint.h>
#include "components/ui/theme.h"

enum {
  SOUND_FX_COIN,

  SOUND_COUNT
};

enum {

  MUSIC_COUNT
};

typedef struct {
  Sound sounds[SOUND_COUNT];
  Font fonts[THEME_FONT_COUNT * THEME_FONTSIZE_COUNT];
  Music musics[MUSIC_COUNT];
} ResourcesState;

void BeginResources();
void EndResources();
