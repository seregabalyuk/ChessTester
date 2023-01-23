
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
    long timers[num_progs];
    int result;
    size_t moves;
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
    for(size_t id = 0; id < num_progs; ++ id) {
        data->timers[id] = 0;
    }
    data->moves = 0;
    data->result = 3;
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

void make_one_move(struct Data* data, size_t move_num) {
    char buffer[SIZE_BUFFER];
    struct Program* prog = &data->programs[move_num % num_progs];
    printf("move: %zu\n", move_num + 1);
    if(move_num % num_progs == 0) {
        printf("white: ");
    } else {
        printf("black: ");
    }
    fsync(prog->dialog.read);
    ssize_t length = read(prog->dialog.read, buffer, SIZE_BUFFER);
    if(length <= 0) {
        printf("\n");
        exit(5);
    }
    buffer[length] = '\0';
    printf("%s", buffer);
    int check = check_move(buffer, move_num % num_progs == 0);
    if(check == 0) {
        struct Program* other_prog = &data->programs[(move_num + 1) % num_progs];
        fsync(other_prog->dialog.write);
        write(other_prog->dialog.write, buffer, length);
        exit(0);
    }
    exit(check);
}

int time_limit = 1 * CLOCKS_PER_SEC;

int make_moves(struct Data* data) {
    size_t move_num = 0;
    for(;move_num < 120; ++ move_num) {
        clock_t start = clock();
        pid_t pid = fork();
        if(pid == 0) {
            make_one_move(data, move_num);
        }
        int wstatus;
        while(waitpid(pid, &wstatus, WNOHANG) == 0) {
            if(clock() - start > time_limit) {
                kill(pid, SIGKILL);
                printf("\n");
                break;
            }
        }
        long time = clock() - start;
        data->timers[move_num % num_progs] += time;
        printf("time: %fsec\n\n", (float)time / CLOCKS_PER_SEC);

        // некоректное поведение программы
        if(time > time_limit || WEXITSTATUS(wstatus) == 4 || WEXITSTATUS(wstatus) == 5) {
            data->result = 1 << ((move_num + 1) % num_progs);
            break;
        }

        // при оканчании легальным способом
        if(WEXITSTATUS(wstatus) != 0) {
            data->result = WEXITSTATUS(wstatus);
            ++ move_num;
            break;
        }
    }
    data->moves = move_num;
    return 0;
}

void draw_result(struct Data* data) {
    printf("status: ");
    switch(data->result) {
    case 1:
        printf("white win\n");
        break;
    case 2:
        printf("black win\n");
        break;
    default:
        printf("draw\n");
    }
    printf("make moves: %zu\n", data->moves);
    printf("white time: %fsec\n", (double) data->timers[0] / CLOCKS_PER_SEC);
    printf("black time: %fsec\n", (double) data->timers[1] / CLOCKS_PER_SEC);
}

int testing(struct Data* data) {
    write_start_position(data);
    make_moves(data);
    draw_result(data);
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
