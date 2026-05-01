#include "stringstack.h"
#include <stdio.h>

const size_t maxSize = 1024 * 32;

void ss_init(StringStack *s) {
    s->buf = NULL;
    s->len = 0;
    s->cap = 0;
    s->stack = NULL;
    s->stack_len = 0;
    s->stack_cap = 0;
}

void ss_reserve(StringStack *s, size_t extra) {
  if(extra >= maxSize) {
    printf("%s", "stringstack overflow");
    exit(1);
  }

  size_t needed = s->len + extra + 1; // +1 for null terminator
  if (needed > s->cap) {
      s->cap = needed * 2;
      s->buf = realloc(s->buf, s->cap);
  }
}

void ss_push_fragment(StringStack *s, const char *str) {
    size_t l = strlen(str);
    ss_reserve(s, l);
    memcpy(s->buf + s->len, str, l);
    s->len += l;
    s->buf[s->len] = '\0';
}

void ss_push(StringStack *s) {
    if (s->stack_len >= s->stack_cap) {
        s->stack_cap = s->stack_cap ? s->stack_cap * 2 : 8;
        s->stack = realloc(s->stack, s->stack_cap * sizeof(size_t));
    }
    s->stack[s->stack_len++] = s->len;
}

void ss_pop(StringStack *s) {
    if (s->stack_len == 0) return;
    s->len = s->stack[--s->stack_len];
    s->buf[s->len] = '\0';
}

const char *ss_str(StringStack *s) {
    return s->buf ? s->buf : "";
}

void ss_free(StringStack *s) {
    free(s->buf);
    free(s->stack);
}