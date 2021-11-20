#pragma once

#define BEBRASH_TOK_BUFSIZE 64
#define BEBRASH_TOK_DELIM " \t\r\n\a"
#define BEBRASH_LINE_DELIM "|"

void greeting();
void bebrash_loop();
char *bebrash_read_String();
char **bebrash_split_String_to_lines(char *String);
char **bebrash_split_line(char *line);
int bebrash_launch(char **tokens);
int bebrash_execute(char **tokens);

/*
  Объявление функций для встроенных команд оболочки:
 */
int bebrash_cd(char **args);
int bebrash_help(char **args);
int bebrash_exit(char **args);

int bebrash_num_builtins();
