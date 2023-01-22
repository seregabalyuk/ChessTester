#include "dialog.h"

void print_table(struct TableFD* table) {
    printf("table {\n");
    for(size_t i = 0; i < max_fds; ++i) {
        printf("  %i\n", table->fds[i]);
    }
    printf("}\n");
}

void create_table(struct TableFD* table) {
    for(size_t i = 0; i < max_fds; ++ i) {
        table->fds[i] = -1;
    }
}

int close_table(struct TableFD* table) {
    for(size_t i = 0; i < max_fds; ++ i) {
        if(table->fds[i] != -1) {
            if(close(table->fds[i])) {
                perror("not close all tablefd\n");
                return -1;
            }
            table->fds[i] = -1;
        }
    }
    return 0;
}

int add_fd(struct TableFD* table, int fd) {
    for(size_t i = 0; i < max_fds; ++ i) {
        if(table->fds[i] == -1) {
            table->fds[i] = fd;
            return 0;
        }
    }
    perror("not enough space in the tablefd\n");
    return -1;
}

int close_fd(struct TableFD* table, int fd) {
    if(close(fd)) { 
        perror("not close one fd in tablefd\n");
        return -1;
    }
    for(size_t i = 0; i < max_fds; ++ i) {
        if(table->fds[i] == fd) {
            table->fds[i] = -1;
            return 0;
        }
    }
    perror("not find fd in tablefd when close one fd\n");
    return -1;
}

int pipe_with_table(struct TableFD* table, struct Pipe* pipe_in) {
    if(pipe((int*)pipe_in)) {
        perror("pipe didnt work\n");
        return -1;
    }
    if(add_fd(table, pipe_in->read)) {
        close(pipe_in->read);
        close(pipe_in->write);
        return -1;
    }
    if(add_fd(table, pipe_in->write)) {
        close_fd(table, pipe_in->read);
        close(pipe_in->write);
        return -1;
    }
    return 0;
}

pid_t open_dialog(struct TableFD* table, struct Pipe* pipe_out, const char* program_name) {
    if(access(program_name, X_OK)) {
        perror("input data is not program\n");
        return -1;
    }
    struct Pipe pin;
    struct Pipe pout;

    if(pipe_with_table(table, &pin)) {
        return -1;
    }
    if(pipe_with_table(table, &pout)) {
        close_fd(table, pin.read);
        close_fd(table, pin.write);
        return -1;
    }
    pid_t pid = fork();
    if(pid == -1) {
        printf("error: flow not created\n");
        close_fd(table, pin.read);
        close_fd(table, pin.write);
        close_fd(table, pout.read);
        close_fd(table, pout.write);
        return -1;
    }
    if(pid == 0) {
        close(0);
        close(1);
        dup(pout.read);
        dup(pin.write);
        close_table(table);
        execlp(program_name, program_name, NULL);
    } else {
        close_fd(table, pout.read);
        close_fd(table, pin.write);
        pipe_out->read = pin.read;
        pipe_out->write = pout.write;
    }
    return pid;
}