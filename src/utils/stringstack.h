#pragma once

#include <stdlib.h>
#include <string.h>

typedef struct {
    char *buf;
    size_t len;
    size_t cap;

    size_t *stack;
    size_t stack_len;
    size_t stack_cap;
} StringStack;

void ss_init(StringStack *s);
void ss_reserve(StringStack *s, size_t extra);
void ss_push_fragment(StringStack *s, const char *str);
void ss_push(StringStack *s);
void ss_pop(StringStack *s);
const char *ss_str(StringStack *s);
void ss_free(StringStack *s);