#pragma once

#include "utils/flex.h"
#include <raylib.h>
#include <stdint.h>

typedef struct {
  bool debugFlex;
} FlexRootConfig;

typedef struct {
  bool absolute, fixed;
  FlexDirection direction;
  FlexAlign align;
  FlexAlign justify;
  float minw;
  float minh;
  float maxw;
  float maxh;
  float w;
  float h;
  float gap;
  float pl;
  float pr;
  float pt;
  float pb;
  float grow;
  float shrink;
} FlexConfig;


void BeginFlexRoot(FlexRootConfig config);
void EndFlexRoot();

void BeginFlex(uint64_t id, FlexConfig config);
void EndFlex(uint64_t id);

void Flex(uint64_t id, FlexConfig config);

Rectangle GetFlexRect();