#include "theme.h"
#include "utils/state.h"

void BeginTheme(Theme initialTheme) {      
    BeginContext(Theme, theme, {});
        BeginInit(Theme)
            *theme = initialTheme;
        EndInit()
}

void EndTheme() {
    EndContext(Theme, theme);
}
