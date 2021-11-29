#include "bebrash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void greeting() {
    printf("Командная оболочка Bebrash запущена.\n");
    printf("Вводите команды:\n");
}

void bebrash_loop() {
    char *String;
    char **commands;
    char ***tokens;
    int status;
    int count_commands;
    int i;
    do {
        printf("> ");
        String = bebrash_read_String();
        commands = bebrash_split_String_to_commands(String, &count_commands);
        tokens = malloc(sizeof(char*) * count_commands);
        for (i = 0; i < count_commands; i++) {
            tokens[i] = bebrash_split_command(commands[i]);
        }
        status = bebrash_launch(tokens, count_commands);
        free(String);
        free(commands);
        free(tokens);

    } while (status);
}

char *bebrash_read_String() {
    char *String = NULL;
    ssize_t bufsize = 0;
    getline(&String, &bufsize, stdin);
    return String;
}

char **bebrash_split_String_to_commands(char *String, int *count_commands) {
    int bufsize = BEBRASH_COMMAND_BUFSIZE;
    int i = 0;
    char **commands = malloc(bufsize * sizeof(char *));
    char *istr;
    if (!commands) {
        fprintf(stderr, "bebrash: ошибка выделения памяти\n");
        exit(EXIT_FAILURE);
    }

    istr = strtok(String, BEBRASH_COMMAND_DELIM);
    while (istr != NULL) {
        commands[i] = istr;
        i++;
        if (i == bufsize) {
            bufsize += BEBRASH_COMMAND_BUFSIZE;
            commands = realloc(commands, bufsize * sizeof(char *));
            if (!commands) {
                fprintf(stderr, "bebrash: ошибка выделения памяти\n");
                exit(EXIT_FAILURE);
            }
        }

        istr = strtok(NULL, BEBRASH_COMMAND_DELIM);
    }
    commands[i] = NULL;
    *count_commands = i;
    return commands;
}

char **bebrash_split_command(char *command) {
    int bufsize = BEBRASH_TOK_BUFSIZE;
    int i = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;
    if (!tokens) {
        fprintf(stderr, "bebrash: ошибка выделения памяти\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(command, BEBRASH_TOK_DELIM);
    while (token != NULL) {
        tokens[i] = token;
        i++;
        if (i == bufsize) {
            bufsize += BEBRASH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char *));
            if (!tokens) {
                fprintf(stderr, "bebrash: ошибка выделения памяти\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, BEBRASH_TOK_DELIM);
    }
    tokens[i] = NULL;
    return tokens;
}

int bebrash_launch(char ***tokens, int count_commands) {

    if (count_commands == 1) {
        int flag = bebrash_execute(tokens[0]);
        if (flag != -1) {
            return flag;
        } else {
            pid_t pid;
            int status;
            pid = fork();
            if (pid == 0) {
                execvp(tokens[0][0], tokens[0]);  
                perror("exec");
                exit(1);
            } else {
                do {
                    waitpid(pid, &status, WUNTRACED);
                } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            }
        }

    } else {
        pid_t pid;
        int fd[count_commands][2];
        int status;
        int step = 0;
        int i;
        for (i = 0; i < count_commands; i++) {
            pipe(fd[i]);
        }
        while (step != count_commands) {
            pid = fork();
            if (step == count_commands - 1) {
                if (pid == 0) {
                    dup2(fd[step][READ_END], STDIN_FILENO); // дублируем дескриптор ввода (теперь ввод идет не из stdin (его мы к тому же закрыли), а из fd[step][READ_END])
                    close(fd[step][WRITE_END]); // закрываем дескриптор для вывода
                    execvp(tokens[step][0], tokens[step]);
                    perror("exec");
                    exit(1);
                }
            } else {
                if (pid == 0) {
                    dup2(fd[step][READ_END], STDIN_FILENO); // дублируем дескриптор ввода (теперь ввод идет не из stdin (его мы к тому же закрыли), а из fd[step][READ_END])
                    close(fd[step][WRITE_END]);
                    dup2(fd[step + 1][WRITE_END], STDOUT_FILENO); // дублируем дескриптор вывода (теперь вывод идет не в stdout (мы его закрыли), а в fd[step+1][WRITE_END])
                    close(fd[step + 1][READ_END]); // закрываем дескриптор для чтения
                    execvp(tokens[step][0], tokens[step]);
                    perror("exec");
                    exit(1);
                }
            }
            close(fd[step + 1][WRITE_END]); // закрываем дескриптор для вывода
            do {
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
            step++;
        }
    }

    return 1;
}

char *builtin_str[] = {"cd", "help", "exit"};

int (*builtin_func[])(char **) = {&bebrash_cd, &bebrash_help, &bebrash_exit};

int bebrash_execute(char **tokens) {
    int i;
    if (tokens[0] == NULL) {
        // Была введена пустая команда.
        return 1;
    }
    for (i = 0; i < bebrash_num_builtins(); i++) {
        if (strcmp(tokens[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(tokens);
        }
    }
    return -1;
}

int bebrash_num_builtins() { return sizeof(builtin_str) / sizeof(char *); }

int bebrash_cd(char **args) {
    if (args[1] == NULL) {
        fprintf(stderr, "bebrash: ожидается аргумент для \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("bebrash");
        }
    }
    return 1;
}

int bebrash_help(char **args) {
    int i;
    printf("Наберите название программы и её аргументы и нажмите enter.\n");
    printf("Вот список встроенных команд:\n");
    for (i = 0; i < bebrash_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }
    printf("Используйте команду man для получения информации по другим "
           "программам.\n");
    return 1;
}

int bebrash_exit(char **args) { return 0; }