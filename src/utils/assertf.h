#pragma once

#define assertf(cond, fmt, ...)                                   \
    do {                                                          \
        if (!(cond)) {                                            \
            fprintf(stderr, "Assertion failed: " fmt "\n",         \
                    ##__VA_ARGS__);                               \
            fprintf(stderr, "  at %s:%d\n", __FILE__, __LINE__);   \
            __builtin_trap();                                     \
            abort();                                              \
        }                                                         \
    } while (0)
