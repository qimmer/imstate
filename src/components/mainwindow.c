
#include "utils/state.h"
#include <raylib.h>

typedef struct {

} MainWindowState;

static void Init(MainWindowState *state) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  //SetConfigFlags(FLAG_VSYNC_HINT);
  SetTraceLogLevel(LOG_WARNING);
  InitWindow(800, 600, "raylib game template");
  InitAudioDevice();

  SetTargetFPS(99999);
}

static void Cleanup(MainWindowState *state) {
  CloseAudioDevice();
  CloseWindow();
}

void BeginMainWindow(uint64_t id, bool *shouldClose) {
  BeginState(id, MainWindowState, state, {
    .onInit = (OnInitCallback)Init,
    .onCleanup = (OnCleanupCallback)Cleanup
  });
    if(shouldClose) *shouldClose = WindowShouldClose();

    BeginDrawing();
        ClearBackground(PINK);
}
void EndMainWindow(uint64_t id) {
    EndDrawing();
  EndState(id);
}