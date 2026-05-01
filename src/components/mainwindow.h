#pragma once

#include <stdbool.h>
#include <stdint.h>

void BeginMainWindow(uint64_t id, bool *shouldClose);
void EndMainWindow(uint64_t id);