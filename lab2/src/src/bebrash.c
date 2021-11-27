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
    int status;
    int count_commands;
    int i;
    do {
        printf("> ");

        String = bebrash_read_String();
        commands = bebrash_split_String_to_commands(String);
        i = 0;
        status = 1;
        while (commands[i] != NULL && status) {
            tokens = bebrash_split_command(commands[i]);
            status = bebrash_execute(tokens);
            i++;
        }
        
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

char **bebrash_split_String_to_commands(char *String) {
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

        if (i == bufsize) { // здесь поставить равно
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

        if (i >= bufsize) { // здесь поставить равно
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