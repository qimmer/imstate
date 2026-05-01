#include "statetree.h"
#include "components/ui/flex.h"
#include "components/ui/tree.h"
#include "utils/flex.h"
#include "utils/state.h"
#include <raylib.h>
#include <stdint.h>
#include <string.h>
/*
static void SubStateTree(uint64_t id, uint64_t scope) {
  const char *scopeName = GetIdName(scope);

  BeginTree(id, (TreeConfig){
    .title = scopeName
  });
  EndTree(id);
}*/

void StateTree(uint64_t id) {
  MakeId(TreeContainer);
/*
  size_t scopeCount;
  uint64_t *scopes = GetAllScopes(&scopeCount);
  const int scopeMaxLen = 512;
  char parentScopeName[scopeMaxLen];

  for(int i = 0; i < scopeCount; ++i) {
    uint64_t scopeId = scopes[i];
    const char *scopeName = GetIdName(scopeId);
    const char* localName = strrchr(scopeName, '/') + 1;
    strncpy(parentScopeName, scopeName, (localName - scopeName) - 1);
    uint64_t parentScope = HashId(parentScopeName);
    
    //SubStateTree(i, scopeId);
  }*/

  BeginId(id);
    BeginFlex(TreeContainer, (FlexConfig){.direction = FlexVertical, .align = FlexStretch, .justify = FlexStart, .grow = 1 });
      for(int i = 0; i < 512; ++i) {
        if(BeginTree(i, (TreeConfig){
          .title = "Hello"//TextFormat("Hello %d", i)
        })) {
          /*for(int j = 0; j < 12; ++j) {
            BeginTree(j, (TreeConfig){
              .title = TextFormat("Hello World %d", j)
            });
            EndTree(j);
          }*/
        }
        EndTree(i);
      }
    EndFlex(TreeContainer);
  EndId(id);
}