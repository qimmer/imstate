#include "textinput.h"
#include "components/resources.h"
#include "components/ui/clipping.h"
#include "components/ui/focus.h"
#include "components/ui/flex.h"
#include "components/ui/panel.h"
#include "components/ui/text.h"
#include "components/ui/theme.h"
#include "utils/flex.h"
#include "utils/state.h"
#include "utils/math.h"

#include <alloca.h>
#include <assert.h>
#include <raylib.h>
#include <stb_ds.h>

typedef char* TextInputString;

static int fontSize = 12;
static float spacing = 0.0;
static Font font;

static void LayoutRow(void *r, TextInputString *obj, int start_i);
static int InsertCharacters(TextInputString *obj, int pos, const char *text, int num);
static float GetTextWidth(TextInputString *obj, int line_start, int char_index);


#define STB_TEXTEDIT_GETWIDTH(obj, line_start, char_index) \
    GetTextWidth((obj), (line_start), (char_index))

#define STB_TEXTEDIT_CHARTYPE char
#define STB_TEXTEDIT_STRING TextInputString
#define STB_TEXTEDIT_STRINGLEN(obj) ((int)arrlen(*obj) - 1)
#define STB_TEXTEDIT_GETCHAR(obj, i) ((*obj)[(i)])
#define STB_TEXTEDIT_INSERTCHARS(obj, pos, text, num) \
    InsertCharacters((obj), (pos), (text), (num))

#define STB_TEXTEDIT_DELETECHARS(obj, pos, num) \
    arrdeln(*obj, pos, num);

#define STB_TEXTEDIT_LAYOUTROW(r, obj, start_i) \
    LayoutRow((r), (obj), (start_i))
#define STB_TEXTEDIT_NEWLINE '\n'
#define STB_TEXTEDIT_KEYTOTEXT(k) (k < 0xfffff000 ? k : 0)

#define STB_TEXTEDIT_K_LEFT        0xfffff100
#define STB_TEXTEDIT_K_RIGHT       0xfffff101
#define STB_TEXTEDIT_K_UP          0xfffff102
#define STB_TEXTEDIT_K_DOWN        0xfffff103
#define STB_TEXTEDIT_K_LINESTART   0xfffff104
#define STB_TEXTEDIT_K_LINEEND     0xfffff105
#define STB_TEXTEDIT_K_TEXTSTART   0xfffff106
#define STB_TEXTEDIT_K_TEXTEND     0xfffff107
#define STB_TEXTEDIT_K_DELETE      0xfffff108
#define STB_TEXTEDIT_K_BACKSPACE   0xfffff109
#define STB_TEXTEDIT_K_UNDO        0xfffff10A
#define STB_TEXTEDIT_K_REDO        0xfffff10B
#define STB_TEXTEDIT_K_WORDLEFT    0xfffff10C
#define STB_TEXTEDIT_K_WORDRIGHT   0xfffff10D
#define STB_TEXTEDIT_K_PGUP        0xfffff10E
#define STB_TEXTEDIT_K_PGDOWN      0xfffff10F
#define STB_TEXTEDIT_K_SHIFT       0xfffff200

#define STB_TEXTEDIT_IMPLEMENTATION

#include <stb_textedit.h>

static void LayoutRow(void *rr, TextInputString *obj, int start_i)
{
    StbTexteditRow *r = (StbTexteditRow*)rr;
    int len = arrlen(*obj);
    char *nterm = alloca(len + 1);
    memcpy(nterm, *obj, len);
    nterm[len] = 0;

    int i = start_i;
    while (i < len && (*obj)[i] != '\n') {
        i++;
    }

    r->num_chars = i - start_i;

    // Measure width (no allocation!)
    float width = MeasureTextEx(
        font,
        nterm + start_i,
        fontSize,
        spacing
    ).x;

    r->x0 = 0.0f;
    r->x1 = width;

    float line_height = fontSize;

    r->baseline_y_delta = line_height;
    r->ymin = 0.0f;
    r->ymax = line_height;
}

static int InsertCharacters(TextInputString *obj, int pos, const char *text, int num)
{
    int oldLen = strlen(*obj);
    arrinsn(*obj, pos, num);

    for (int i = 0; i < num; i++) {
      (*obj)[pos + i] = text[i];
    }

    assert((oldLen + num) == strlen(*obj));

    return num;
}

static float GetTextWidth(TextInputString *obj, int line_start, int char_index)
{
  TextInputString str = *obj;
    char c = str[line_start + char_index];
    char tmp[2] = { c, 0 };

    return MeasureTextEx(font, tmp, fontSize, spacing).x;
}

typedef struct {
  STB_TexteditState stbState;
  char *str;
  bool dragging;
} TextInputState;

static void Init(TextInputState *state, TextInputConfig *config) {
  stb_textedit_initialize_state(&state->stbState, config->multiline ? 0 : 1);
  if(config->initialValue) {
    int len = strlen(config->initialValue);
    arrsetlen(state->str, len + 1);
    memcpy(state->str, config->initialValue, len);
    state->str[len] = '\0';
  } else {
    arrpush(state->str, '\0');
  }
}

static void Cleanup(TextInputState* state) {
  arrfree(state->str);
}

static Vector2 GetCursorPosition(const char *text, int cursor)
{
    int i = 0;
    int line_start = 0;
    int row = 0;
    int column = 0;

    while (i < cursor) {
        if (text[i] == '\n') {
            row++;
            column = 0;
            line_start = i + 1;
        }
        i++;
        column++;
    }

    char *lineTextUntilCursor = alloca(column + 1);
    memcpy(lineTextUntilCursor, text + line_start, column);
    lineTextUntilCursor[column] = '\0';

    // X = width from line_start → cursor
    float x = MeasureTextEx(
        font,
        lineTextUntilCursor,
        fontSize,
        spacing
    ).x;

    float y = row * fontSize;

    return (Vector2){ x, y };
}

void TextInput(uint64_t id, TextInputConfig config) {
  UseContext(Theme, theme);
  UseContext(ResourcesState, resources);

  config.flex.justify = FlexStretch;
  config.flex.align = FlexCenter;
  config.flex.direction = FlexHorizontal;
  config.flex.pl = theme->spacings[THEME_SPACING_S];
  config.flex.pr = theme->spacings[THEME_SPACING_S];

  if(config.multiline) {
    config.flex.pt = theme->spacings[THEME_SPACING_XS];
    config.flex.pb = theme->spacings[THEME_SPACING_XS];
  } else {
    config.flex.minh = theme->sizes[THEME_SIZE_M];
  }
  
  BeginState(id, TextInputState, state, { .config = &config, .onInit = (OnInitCallback)Init, .onCleanup = (OnCleanupCallback)Cleanup});
    BeginFocusable(id);
      bool isFocused = IsFocused();

      BeginPanel(id, (PanelConfig){
        .flex = config.flex,
        .color = isFocused ? theme->colors[THEME_COLOR_BACKGROUND_INPUT] : theme->colors[THEME_COLOR_BACKGROUND_HOVER],
      });
        Rectangle rect = GetFlexRect();
        bool isHovering = !IsPointClipped((Vector2) { .x = GetMouseX(), .y = GetMouseY() });
        

        if(isFocused) {
          DrawRectangleLines(rect.x, rect.y, rect.width, rect.height, theme->colors[THEME_COLOR_FOREGROUND]);
        }

        font = resources->fonts[THEME_FONTSIZE_COUNT * config.textConfig.themeFont + config.textConfig.themeFontSize];
        fontSize = theme->fontSizes[config.textConfig.themeFontSize] * theme->fonts[config.textConfig.themeFont].size;
        spacing = 0.0f;

        if(!state->stbState.single_line != config.multiline) {
          stb_textedit_clear_state(&state->stbState, config.multiline ? 0 : 1);
        }

        bool changed = false;
        int c;

        if(isFocused) {
          while ((c = GetCharPressed()) != 0) {
              stb_textedit_key(&state->str, &state->stbState, c);
              changed = true;
          }

          const int keys[][2] = {
            { KEY_LEFT, (IsKeyDown(KEY_LEFT_CONTROL) ? STB_TEXTEDIT_K_WORDLEFT : STB_TEXTEDIT_K_LEFT) },
            { KEY_RIGHT, (IsKeyDown(KEY_LEFT_CONTROL) ? STB_TEXTEDIT_K_WORDLEFT : STB_TEXTEDIT_K_RIGHT) },
            { KEY_UP, STB_TEXTEDIT_K_UP },
            { KEY_DOWN, STB_TEXTEDIT_K_DOWN },
            { KEY_BACKSPACE, STB_TEXTEDIT_K_BACKSPACE },
            { KEY_PAGE_UP, STB_TEXTEDIT_K_PGUP },
            { KEY_PAGE_DOWN, STB_TEXTEDIT_K_PGDOWN },
            { KEY_DELETE, STB_TEXTEDIT_K_DELETE },
            { KEY_HOME, (IsKeyDown(KEY_LEFT_CONTROL) ? STB_TEXTEDIT_K_TEXTSTART : STB_TEXTEDIT_K_LINESTART) },
            { KEY_END, (IsKeyDown(KEY_LEFT_CONTROL) ? STB_TEXTEDIT_K_TEXTEND : STB_TEXTEDIT_K_LINEEND) },
          };

          for(int i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
            if(IsKeyPressed(keys[i][0]) || IsKeyPressedRepeat(keys[i][0])) {
              stb_textedit_key(&state->str, &state->stbState, keys[i][1] | (IsKeyDown(KEY_LEFT_SHIFT) ? STB_TEXTEDIT_K_SHIFT : 0));
            }
          }

          if (IsKeyPressed(KEY_ENTER)) {
              stb_textedit_key(&state->str, &state->stbState, '\n');
              changed = true;
          }

          if(changed && config.OnValueChange) {
            config.OnValueChange(config.context, state->str);
          }
        }
        
        Rectangle textRect = Text(0, state->str, { .themeColor = THEME_COLOR_FOREGROUND_INPUT, .flex = { .grow = 1, .h = theme->sizes[THEME_SIZE_S], .minh = theme->sizes[THEME_SIZE_S] }});

        if(IsMouseButtonPressed(0)) {
          if(isHovering) {
            stb_textedit_click(&state->str, &state->stbState, GetMouseX() - textRect.x, GetMouseY() - textRect.y);
            Focus();
            state->dragging = true;
          } else {
            if(IsFocused()) {
              Blur();
            }
          }
        }

        if(IsMouseButtonUp(0)) {
          state->dragging = false;
        }

        if(state->dragging) {
          stb_textedit_drag(&state->str, &state->stbState, GetMouseX() - textRect.x, GetMouseY() - textRect.y);
        }

        // Cursor
        if(isFocused) {
          Vector2 cursor = GetCursorPosition(state->str, state->stbState.cursor);
          DrawRectangle(textRect.x + cursor.x, textRect.y + cursor.y, 2, textRect.height, theme->colors[THEME_COLOR_FOREGROUND_INPUT]);
        }

        // Selection rectangle
        if(state->stbState.select_start != state->stbState.select_end && isFocused) {
          Vector2 selectStart = GetCursorPosition(state->str, state->stbState.select_start);
          Vector2 selectEnd = GetCursorPosition(state->str, state->stbState.select_end);
          int selectionLen = abs(state->stbState.select_end - state->stbState.select_start);
          Rectangle selectionRect = {
            .x = textRect.x + fmin(selectStart.x, selectEnd.x), 
            .y = textRect.y + fmin(selectStart.y, selectEnd.y), 
            .width = fabs(selectEnd.x - selectStart.x),
            .height = fabs(selectEnd.y - selectStart.y) + fontSize
          };

          DrawRectangle(
            selectionRect.x,
            selectionRect.y,
            selectionRect.width,
            selectionRect.height,
            theme->colors[THEME_COLOR_ACCENT]
          );
          
          char* part = alloca(selectionLen + 1);
          memcpy(part, state->str + MIN(state->stbState.select_end, state->stbState.select_start), selectionLen);
          part[selectionLen] = '\0';
          
          DrawTextEx(font, part, (Vector2){ .x = selectionRect.x , .y = selectionRect.y  }, fontSize, spacing, theme->colors[THEME_COLOR_ACCENT_FOREGROUND]);
        }
      EndPanel(id);
    EndFocusable(id);
  EndState(id);
}
