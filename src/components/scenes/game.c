#include <raylib.h>

#include "utils/state.h"
#include "components/ui/clipping.h"
#include "../resources.h"
#include "../test/cubes.h"
#include "game.h"

typedef struct {
    Camera3D camera;
    GameConfig config;
} GameScreenState;

static void InitGame(GameScreenState *s) {
  *s = (GameScreenState) {
    .camera = {
      .fovy = 45.f,
      .up = (Vector3){ 0.0f, 1.0f, 0.0f },
      .target = (Vector3){ 0.0f, 0.0f, 0.0f },
      .position = (Vector3){ 0.0f, 0.0f, 150.0f },
      .projection = CAMERA_PERSPECTIVE
    }
  };
}

void BeginGame(uint64_t id, GameConfig config) {
  UseContext(ResourcesState, resources);
  BeginId(id);
    BeginContext(GameScreenState, state, {.onInit = (OnInitCallback)InitGame});
      state->config = config;
      if(config.target.id) {
        BeginTextureMode(config.target);
      }
      ClearBackground(RED);

      BeginMode3D(state->camera);
          Cubes(HashId("cubes"));
    
}

void EndGame(uint64_t id) {
  UseContext(ResourcesState, resources);
  UseContext(GameScreenState, state);
      EndMode3D();

      if(state->config.target.id) {
        EndTextureMode();
      }

      if(IsHovering()) {
        if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
            PlaySound(resources->sounds[SOUND_FX_COIN]);
        }
      }
      
    EndContext(GameScreenState, state);
  EndId(id);
}