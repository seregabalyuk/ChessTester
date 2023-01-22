#ifndef TESTER_DIALOG_H
#define TESTER_DIALOG_H
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define max_fds 16

struct Pipe {
    int read;
    int write;
};

struct TableFD {
    int fds[max_fds];
};

void create_table(struct TableFD* table);

int close_table(struct TableFD* table);

int add_fd(struct TableFD* table, int fd);

int close_fd(struct TableFD* table, int fd);

int pipe_with_table(struct TableFD* table, struct Pipe* pipe_in);

pid_t open_dialog(struct TableFD* table, struct Pipe* pipe_out, const char* program_name);

#endif