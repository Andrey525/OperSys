#pragma once

void greeting();
void bebrash_loop();
char *bebrash_read_line();
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
