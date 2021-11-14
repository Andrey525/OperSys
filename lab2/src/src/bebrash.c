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
    char *line;
    char **tokens;
    int status;
    do {
        printf("> ");

        line = bebrash_read_line();
        tokens = bebrash_split_line(line);
        status = bebrash_execute(tokens);

        free(line);
        free(tokens);
    } while (status);
}

char *bebrash_read_line() {
    char *line = NULL;
    ssize_t bufsize = 0; // getline сама выделит память
    getline(&line, &bufsize, stdin);
    return line;
}

#define BEBRASH_TOK_BUFSIZE 64
#define BEBRASH_TOK_DELIM " \t\r\n\a"

char **bebrash_split_line(char *line) {
    int bufsize = BEBRASH_TOK_BUFSIZE;
    int i = 0;
    char **tokens = malloc(bufsize * sizeof(char *));
    char *token;

    if (!tokens) {
        fprintf(stderr, "bebrash: ошибка выделения памяти\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, BEBRASH_TOK_DELIM);
    while (token != NULL) {
        tokens[i] = token;
        i++;

        if (i >= bufsize) {
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
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Дочерний процесс
        if (execvp(tokens[0], tokens) == -1) {
            perror("bebrash");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // Ошибка при форкинге
        perror("bebrash");
    } else {
        // Родительский процесс
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
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

/*
  Реализации встроенных функций
*/
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