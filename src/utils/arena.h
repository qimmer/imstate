
//
// 🔹 Simple arena allocator (persistent)
//
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stb_ds.h>

typedef struct {
    uint8_t *data;
} Arena;

void *ArenaAlloc(Arena *arena, size_t size) {
    size = (size + 7) & ~7; // align 8

    arrsetlen(arena->data, arrlen(arena->data) + size);
    void *ptr = arena->data + arrlen(arena->data) - size;
    
    return ptr;
}

void ArenaReset(Arena *arena) {
    arrsetlen(arena->data, 0);
}