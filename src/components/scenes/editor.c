#include "components/ui/editor/statetree.h"
#include "components/ui/text.h"
#include "components/ui/textinput.h"
#include "utils/flex.h"
#include "utils/state.h"
#include "game.h"
#include "components/ui/flex.h"
#include "components/ui/window.h"
#include "components/ui/theme.h"
#include <raylib.h>

typedef struct {
  uint64_t modelId;

  RenderTexture2D gameBuffer;
} EditorState;

static void Init(EditorState *state) {
  state->gameBuffer = LoadRenderTexture(512, 512);
}

static void Cleanup(EditorState *state) {
  UnloadRenderTexture(state->gameBuffer);
}

void BeginEditor(uint64_t id, RenderTexture2D *viewportOut) {
  MakeId(EditorRoot);
  MakeId(TopBar);
  MakeId(CenterRegion);
  MakeId(LeftBar);
  MakeId(CenterWindow);
  MakeId(GameId);
  MakeId(StateTreeId);
  MakeId(Search);

  UseContext(Theme, theme);

  BeginState(id, EditorState, state, { .onInit = (OnInitCallback)Init, .onCleanup = (OnCleanupCallback)Cleanup });
    BeginFlex(EditorRoot, (FlexConfig){
      .direction = FlexVertical,
      .align = FlexStretch,
      .justify = FlexStretch,
      .grow = 1
    });      
      BeginWindow(TopBar, (WindowConfig){
        .noPadding = true,
        .flex = {
          .h = theme->sizes[THEME_SIZE_M]
        }
      });
      EndWindow(TopBar);
      BeginFlex(CenterRegion, (FlexConfig){
        .grow = 1,
        .direction = FlexHorizontal,
        .align = FlexStretch,
        .justify = FlexStretch,
      });
        BeginWindow(LeftBar, (WindowConfig){
          .noPadding = true,
          .flex = {
            .direction = FlexVertical,
            .align = FlexStretch,
            .w = theme->sizes[THEME_SIZE_2XL]
          }
        });
          TextInput(Search, (TextInputConfig) {
            .initialValue = "Hello",
          });

          StateTree(StateTreeId);
        EndWindow(LeftBar);

        BeginFlex(CenterWindow, (FlexConfig){
          .grow = 1
        });
          Rectangle rect = GetFlexRect();
          bool resized = state->gameBuffer.texture.width != rect.width || state->gameBuffer.texture.height != rect.height;
          if(resized && rect.width > 16 && rect.height > 16) {
            UnloadRenderTexture(state->gameBuffer);
            state->gameBuffer = LoadRenderTexture(rect.width, rect.height);
          }

          if(viewportOut) *viewportOut = state->gameBuffer;
}

void EndEditor(uint64_t id) {
  MakeId(EditorRoot);
  MakeId(CenterRegion);
  MakeId(CenterWindow);
  MakeId(GameId);
  MakeId(RightBar);
  MakeId(BottomBar);
  MakeId(FpsText);
  MakeId(FrametimeText);

  UseContext(Theme, theme);
  UseState(id, EditorState, state);

          Rectangle rect = GetFlexRect();

          DrawTextureRec(
            state->gameBuffer.texture, 
            (Rectangle){0, 0, rect.width, -rect.height}, // flip Y
            (Vector2){rect.x, rect.y},
            WHITE
          );
        EndFlex(CenterWindow);

        BeginWindow(RightBar, (WindowConfig){
          .flex = {
            .w = theme->sizes[THEME_SIZE_2XL]
          }
        });
        EndWindow(RightBar);
      EndFlex(CenterRegion);
      BeginWindow(BottomBar, (WindowConfig){
        .noPadding = true,
        .flex = {
          .h = theme->sizes[THEME_SIZE_S],
          .gap = theme->spacings[THEME_SPACING_XS],
          .pl = theme->spacings[THEME_SPACING_XS],
          .pr = theme->spacings[THEME_SPACING_XS],
          .align = FlexCenter,
          .direction = FlexHorizontal
        }
      });
        const char *fps = TextFormat("FPS: %d", GetFPS());
        const char *frameTime = TextFormat("Frame Time: %.1fms", GetFrameTime() * 1000.0f);
        Text(FpsText, fps, { .themeFontSize = THEME_FONTSIZE_M, .themeFont = THEME_FONT_MONO, .flex = { .w = theme->sizes[THEME_SIZE_L], .justify = FlexStart } });
        Text(FrametimeText, frameTime, { .themeFontSize = THEME_FONTSIZE_M, .themeFont = THEME_FONT_MONO, .flex = { .w = theme->sizes[THEME_SIZE_L], .justify = FlexStart } });

      EndWindow(BottomBar);
    EndFlex(EditorRoot);
  EndState(id);
}