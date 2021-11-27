#include "bebrash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

void greeting() {
    printf("The bebrash command shell is running.\n");
    printf("Enter your commands:\n");
}

void bebrash_loop() {
    char *String;
    char **commands;
    char **tokens;
    char **tokens2;
    int status;
    int count_commands;
    int i;
    int fds1[2], fds2[2];
    do {
        printf("> ");
        pipe(fds1);
        pipe(fds2);
        String = bebrash_read_String();
        commands = bebrash_split_String_to_commands(String, &count_commands);
        if (count_commands == 1) {
            tokens = bebrash_split_command(commands[0]);
            status = bebrash_execute(tokens);
            free(String);
            free(commands);
            free(tokens);
        } else if (count_commands == 2) {
            tokens = bebrash_split_command(commands[0]);
            tokens2 = bebrash_split_command(commands[1]);
            status = bebrash_launch_pipe(tokens, tokens2);
            free(String);
            free(commands);
            free(tokens);
        } else {
            printf("Sorry, Bebrash cannot execute more than 2 commands!\n");
            free(String);
            free(commands);
        }
    } while (status);
}

char *bebrash_read_String() {
    char *String = NULL;
    ssize_t bufsize = 0;
    getline(&String, &bufsize, stdin);
    return String;
}

char **bebrash_split_String_to_commands(char *String, int *count_commands) {
    int bufsize = 10;
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
            bufsize += 10;
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

int bebrash_launch(char **tokens) {
    pid_t pid;
    int status;
    pid = fork();
    if (pid == 0) {
        // Дочерний процесс
        if (execvp(tokens[0], tokens) == -1) {
            perror("exec");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Ошибка при форкинге
        perror("fork");
    } else {
        // Родительский процесс
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int bebrash_launch_pipe(char **tokens, char **tokens2) {
    pid_t pid;
    int fd[2];
    int status;
    pipe(fd);
    pid = fork();
    if (pid == 0) {
        dup2(fd[WRITE_END], STDOUT_FILENO);
        close(fd[READ_END]);
        close(fd[WRITE_END]);
        execvp(tokens[0], tokens);
        perror("exec1");
        exit(1);
    } else {
        pid = fork();
        if (pid == 0) {
            dup2(fd[READ_END], STDIN_FILENO);
            close(fd[WRITE_END]);
            close(fd[READ_END]);
            execvp(tokens2[0], tokens2);
            perror("exec2");
            exit(1);
        } else {
            int status;
            close(fd[READ_END]);
            close(fd[WRITE_END]);
            waitpid(pid, &status, 0);
        }
    }
    return 1;
}

/*
  Список встроенных команд, за которыми следуют соответствующие функции
 */
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
    return bebrash_launch(tokens);
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
    printf("Вот список втсроенных команд:\n");
    for (i = 0; i < bebrash_num_builtins(); i++) {
        printf("  %s\n", builtin_str[i]);
    }
    printf("Используйте команду man для получения информации по другим "
           "программам.\n");
    return 1;
}

int bebrash_exit(char **args) { return 0; }