
#include "tester.h"
#define num_progs 2
//const size_t num_progs = 2;
const size_t SIZE_BUFFER = 4096;
const char default_log_name[] = "log.txt";

struct Program {
    const char* name;
    pid_t pid;
    struct Pipe dialog;
};

struct Data {
    struct TableFD table;
    struct Program programs[num_progs];
};

int preparation(struct Data* data, int argc, char *argv[]) {
    create_table(&data->table);
    size_t id_program = 0;
    for(size_t i = 1; i < argc; ++ i) {
        if(id_program < num_progs) {
            data->programs[id_program ++].name = argv[i];
        }
    }
    if(id_program < num_progs) {
        printf("not enough parameters\n");
        return -1;
    }
    for(size_t id = 0; id < num_progs; ++ id) {
        struct Program* prg = &data->programs[id];
        prg->pid = open_dialog(&data->table, &prg->dialog, prg->name);
        if(prg->pid < 0) {
            perror("in preparation\n");
            return -1;
        }
    }
    return 0;
}

int write_start_position(struct Data* data) {
    char* start_position = get_start_position();
    printf("white = %s\n", data->programs[0].name);
    printf("black = %s\n", data->programs[1].name);
    printf("%s\n", start_position);

    fsync(data->programs[0].dialog.write);
    write(data->programs[0].dialog.write, "white\n", sizeof("white\n") - 1);
    fsync(data->programs[1].dialog.write);
    write(data->programs[1].dialog.write, "black\n", sizeof("black\n") - 1);
    for(size_t i = 0; i < 8; ++i) {
        for(size_t id = 0; id < num_progs; ++id) {
            fsync(data->programs[id].dialog.write);
            write(data->programs[id].dialog.write, start_position + 9 * i, 9);
        }
    }
    free(start_position);
    return 0;
}

int time_limit = 1 * CLOCKS_PER_SEC;

int make_moves(struct Data* data) {
    char buffer[SIZE_BUFFER];
    size_t move_num = 0;
    long timers[num_progs];
    for(size_t id = 0; id < num_progs; ++ id) {
        timers[id] = 0;
    }
    while(move_num <= 120) {
        for(size_t id = 0; id < num_progs; ++ id) {
            struct Program* prog = &data->programs[id];
            clock_t start = clock();
            pid_t pid = fork();
            if(pid == 0) {
                printf("move: %zu\n", ++ move_num);
                if(id == 0) {
                    printf("white: ");
                } else {
                    printf("black: ");
                }

                fsync(prog->dialog.read);
                ssize_t length = read(prog->dialog.read, buffer, SIZE_BUFFER);
                if(length <= 0) {
                    printf("\n");
                    close_table(&data->table);
                    exit(1);
                }
                printf("%.*s", (int)length, buffer);

                for(size_t other_id = 0; other_id < num_progs; ++ other_id) {
                    if(id != other_id) {
                        struct Program* other_prog = &data->programs[id];
                        fsync(other_prog->dialog.write);
                        write(other_prog->dialog.write, buffer, length);
                    }
                }
                close_table(&data->table);
                exit(0);
            }
            int wstatus;
            while(waitpid(pid, &wstatus, WNOHANG) == 0) {
                if(clock() - start > time_limit) {
                    kill(pid, SIGKILL);
                    break;
                }
            }
            long time = clock() - start;
            timers[id] += time;
            printf("time: %fsec\n\n", (float)time / CLOCKS_PER_SEC);
            if(time > time_limit || WEXITSTATUS(wstatus) == 1) {
                goto stop;
            }
        }
    }
stop:
    printf("stats\n");
    printf("winner: \n");
    printf("number moves: %zu\n", move_num);
    printf("white time: %fsec\n", (float)timers[0] / CLOCKS_PER_SEC);
    printf("black time: %fsec\n", (float)timers[1] / CLOCKS_PER_SEC);
    return 0;
}

int testing(struct Data* data) {
    write_start_position(data);
    make_moves(data);
    return 0;
}

int main(int argc, char *argv[]) {
    struct Data data;
    if(preparation(&data, argc, argv)) {
        return -1;
    }
    if(testing(&data)) {
        close_table(&data.table);
        return -1;
    }
    close_table(&data.table);
    return 0;
}
