
//
// 🔹 Simple arena allocator (persistent)
//
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uint8_t *data;
    size_t capacity;
    size_t offset;
} Arena;

void ArenaInit(Arena *arena, size_t size) {
    arena->data = malloc(size);
    arena->capacity = size;
    arena->offset = 0;
}

void *ArenaAlloc(Arena *arena, size_t size) {
    size = (size + 7) & ~7; // align 8

    if (arena->offset + size > arena->capacity) {
        fprintf(stderr, "Arena out of memory\n");
        exit(1);
    }

    void *ptr = arena->data + arena->offset;
    arena->offset += size;
    return ptr;
}
