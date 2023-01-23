#ifndef TESTER_RULECHECKER_H
#define TESTER_RULECHECKER_H

char* get_start_position();

// 0 - ход корректен и игра продолжается
// 1 - ход корректен и белые выигрывают
// 2 - ход корректен и выигрывают чёрные
// 3 - ход корректен и происходит ничья
// 4 - ход не корректен
int check_move(const char* move, int is_white);


#endif