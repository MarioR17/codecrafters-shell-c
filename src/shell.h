#ifndef SHELL_H
#define SHELL_H

#define MAX_BUFF_SIZE 128
#define MAX_COMMANDS 64
#define SHELL_EXIT 0
#define SHELL_CONTINUE 1
#define TRUE 1
#define FALSE 0
#define FOUND 1
#define NOT_FOUND 0

// builtin commands
struct builtin {
        char *name;
        unsigned char (*func)(char *args);
};

unsigned char process_command(char *buffer);
unsigned char cmd_exit(char *args);
unsigned char cmd_echo(char *args);
unsigned char cmd_type(char *args);
void type_single_cmd(char *command);
unsigned char is_builtin_cmd(char *command);
unsigned char builtin_name_to_idx(char *command);
unsigned char get_exec_command_path(char *buffer, int buff_size, char *command);

#endif // SHELL_H
