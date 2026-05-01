#include <stddef.h>
#include <stdint.h>

inline static size_t next_power_of_two(size_t n) {
    if (n == 0) return 1;

    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    
    if(sizeof(size_t) > sizeof(uint32_t)) {
      n |= n >> 32;      
    }

    n++;

    return n;
}