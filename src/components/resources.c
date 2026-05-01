#include "resources.h"

#include "utils/state.h"
#include <raylib.h>

static void Init(ResourcesState *resourcesState) {
  UseContext(Theme, theme);

  for(int fontIdx = 0; fontIdx < THEME_FONT_COUNT; ++fontIdx) {
    for(int fontSizeIdx = 0; fontSizeIdx < THEME_FONTSIZE_COUNT; ++fontSizeIdx) {
      resourcesState->fonts[THEME_FONTSIZE_COUNT * fontIdx + fontSizeIdx] = LoadFontEx(theme->fonts[fontIdx].filePath, theme->fontSizes[fontSizeIdx] * theme->fonts[fontIdx].size, theme->fonts[fontIdx].codePoints, theme->fonts[fontIdx].codePointsCount);    
    }  
  }
  resourcesState->sounds[SOUND_FX_COIN] = LoadSound("resources/coin.wav");
}

static void Cleanup(ResourcesState *state) {
  for(int i = 0; i < THEME_FONTSIZE_COUNT * THEME_FONT_COUNT; ++i) {
    UnloadFont(state->fonts[i]);
  }
  for(int i = 0; i < MUSIC_COUNT; ++i) {
    UnloadMusicStream(state->musics[i]);
  }
  for(int i = 0; i < SOUND_COUNT; ++i) {
    UnloadSound(state->sounds[i]);
  }
}

void BeginResources() {
  BeginContext(ResourcesState, resourcesState, { .onInit = (OnInitCallback)Init, .onCleanup = (OnCleanupCallback)Cleanup });
}

void EndResources() {
  EndContext(ResourcesState, resourcesState);
}
