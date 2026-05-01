#pragma once

#include "utils/state.h"

void BeginFocusRoot();
void EndFocusRoot();

void BeginFocusable(StateId id);
void EndFocusable(StateId id);

void Focus();
void Blur();

bool IsFocused();