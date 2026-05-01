#include "utils/state.h"

#include "stb_ds.h"
#include <raylib.h>
#include <rlgl.h>
#include <stdlib.h>

typedef struct {
  float rotation;
  float x;
  float y;
} CubeState;

static void Init(CubeState *state) {
  state->x = (float)(50 - rand() % 100);
  state->y = (float)(50 - rand() % 100);
  state->rotation = (float)(rand() % 360);
}

void RotatingCube(uint64_t id) {
  BeginState(id, CubeState, state, { .onInit = (OnInitCallback)Init });
    state->rotation += GetFrameTime() * 90.0f;

    rlPushMatrix();
      rlTranslatef(state->x, state->y, 0.0f);
      rlRotatef(state->rotation, 0.0f, 1.0f, 0.0f);

      DrawCube((Vector3){0,0,0}, 2.0f, 2.0f, 2.0f, YELLOW);
      DrawCubeWires((Vector3){0,0,0}, 2.0f, 2.0f, 2.0f, BLACK);

    rlPopMatrix();
  EndState(id);
}

void Cubes(uint64_t id) {
  MakeId(CubeId);
  BeginFor(id, i, 3);
    RotatingCube(CubeId);
  EndFor(id, i);
}
