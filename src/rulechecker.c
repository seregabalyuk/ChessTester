#include <malloc.h>
#include "rulechecker.h"

char* get_start_position() {
    char* out = malloc(9 * 8 + 1);
    for(size_t i = 0; i < 9 * 8; ++ i) {
        out[i] = '.';
    }
    for(size_t i = 0; i < 8; ++ i) {
        out[i * 9 + 8] = '\n';
    }
    out[9 * 8] = '\0';
    return out;
}