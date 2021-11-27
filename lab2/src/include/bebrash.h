#pragma once

#define BEBRASH_TOK_BUFSIZE 64
#define BEBRASH_TOK_DELIM " \t\r\n\a"
#define BEBRASH_COMMAND_DELIM "|"
#define WRITE_END 1
#define READ_END 0

void greeting();
void bebrash_loop();
char *bebrash_read_String();
char **bebrash_split_String_to_commands(char *String, int *count_commands);
char **bebrash_split_command(char *command);
int bebrash_launch(char **tokens);
int bebrash_execute(char **tokens);
int bebrash_launch_pipe(char **tokens, char **tokens2);

int bebrash_cd(char **args);
int bebrash_help(char **args);
int bebrash_exit(char **args);

int bebrash_num_builtins();
