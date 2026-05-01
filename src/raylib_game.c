#include "components/app.h"
#include "components/scenes/editor.h"
#include "components/scenes/game.h"
#include "utils/state.h"

#define STB_DS_IMPLEMENTATION
#define RAYGUI_IMPLEMENTATION
#include <raylib.h>
#include <raygui.h>
#include "stb_ds.h"

static bool Frame(void)
{
  MakeId(Editor);
  MakeId(Game);
  MakeId(RaylibGameApp);

  RenderTexture2D viewport;
  bool shouldClose = false;
  
  BeginFrame();
      BeginApp(RaylibGameApp, &shouldClose);
        BeginEditor(Editor, &viewport);
          BeginGame(Game, (GameConfig) { .target = viewport });
          EndGame(Game);
        EndEditor(Editor);
      EndApp(RaylibGameApp);
  EndFrame();

  return shouldClose;
}

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>

int main(void)
{
  StateContext stateContext;
  CreateStateContext(&stateContext);
  SetStateContext(&stateContext);
  emscripten_set_main_loop(Frame, 60, 1);
  DestroyStateContext(&stateContext);
  return 0;
}
#else
int main(void)
{
  StateContext stateContext;
  CreateStateContext(&stateContext, 1024*1024, 4096);
  SetStateContext(&stateContext);
  while (!Frame()) {}
  DestroyStateContext(&stateContext);
  return 0;
}
#endif
