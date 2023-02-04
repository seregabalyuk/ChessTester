#include <malloc.h>
#include "rulechecker.h"
#include <stdlib.h>
#include <time.h>

char map[8][8];

static char* draw() {
    char* out = malloc(9 * 8 + 1);
    for(size_t i = 0; i < 8; ++ i) {
        for(size_t j = 0; j < 8; ++ j) {
            out[i * 9 + j] = map[i][j];
        }
    }
    for(size_t i = 0; i < 8; ++ i) {
        out[i * 9 + 8] = '\n';
    }
    out[9 * 8] = '\0';
    return out;
}

static void map_clear() {
    for(size_t i = 0; i < 8; ++ i) {
        for(size_t j = 0; j < 8; ++ j) {
            map[i][j] = '.';
        }
    }
}

static char* to_put_on(char* array, char empty, int pos) {
    int  i = 0;
    while(1) {
        if(array[i] == empty) {
            if(pos == 0) {
                break;
            }
            -- pos;
        }
        ++ i;
    }
    return array + i;
}

char* get_start_position() {
    map_clear();
    srand(time(0));
    map[0][(rand() % 4) * 2] = 'b';
    map[0][(rand() % 4) * 2 + 1] = 'b';
    *to_put_on(map[0], '.', rand() % 6) = 'q';
    *to_put_on(map[0], '.', rand() % 5) = 'n';
    *to_put_on(map[0], '.', rand() % 4) = 'n';
    *to_put_on(map[0], '.', 0) = 'r';
    *to_put_on(map[0], '.', 0) = 'k';
    *to_put_on(map[0], '.', 0) = 'r';  
    for(int j = 0; j < 8; ++ j) {
        map[1][j] = 'p';
        map[6][j] = 'P';
        map[7][j] = map[0][j] + 'A' - 'a';
    }
    return draw();
}

// 0 - ход корректен и игра продолжается
// 1 - ход корректен и белые выигрывают
// 2 - ход корректен и выигрывают чёрные
// 3 - ход корректен и происходит ничья
// 4 - ход не корректен
int check_move(const char* move, int is_white) {

    return 0;
}