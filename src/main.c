#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define MAX_BUFF_SIZE 128
#define MAX_COMMANDS 64
#define SHELL_EXIT 0
#define SHELL_CONTINUE 1
#define TRUE 1
#define FALSE 0

unsigned char process_command(char *buffer);
unsigned char cmd_exit(char *args);
unsigned char cmd_echo(char *args);
unsigned char cmd_type(char *args);
void type_single_cmd(char *command);
unsigned char is_builtin_cmd(char *command);
unsigned char builtin_name_to_idx(char *command);

struct builtin {
        char *name;
        unsigned char (*func)(char *args);
};

static struct builtin builtins[] = {
        {"exit", cmd_exit},
        {"echo", cmd_echo},
        {"type", cmd_type},
        {NULL, NULL}
};

int main(int argc, char *argv[])
{
        // Flush after every printf
        setbuf(stdout, NULL);

        char buffer[MAX_BUFF_SIZE];

        while (1) {
                printf("$ ");

                if (!fgets(buffer, sizeof(buffer), stdin))
                        break;
                buffer[strcspn(buffer, "\n")] = '\0';

                unsigned char status = process_command(buffer);

                if (status == SHELL_EXIT)
                        break;
        }

        return 0;
}

unsigned char process_command(char *buffer)
{
        char *command = strtok(buffer, " \n\t");

        if (!command)
                return SHELL_CONTINUE;

        char *args = strtok(NULL, "");

        if (is_builtin_cmd(command)) {
                return builtins[builtin_name_to_idx(command)].func(args);
        }

        printf("%s: command not found\n", command);
        return SHELL_CONTINUE;
}

unsigned char cmd_exit(char *args)
{
        return SHELL_EXIT;
}

unsigned char cmd_echo(char *args)
{
        if (args)
                printf("%s\n", args);

        return SHELL_CONTINUE;
}

unsigned char cmd_type(char *args)
{
        char *commands[MAX_COMMANDS];
        int cmd_count = 0;
        char *cmd_token = strtok(args, " \n\t");

        while (cmd_token && cmd_count < (MAX_COMMANDS - 1)) {
                commands[cmd_count++] = cmd_token;
                cmd_token = strtok(NULL, " \n\t");
        }

        for (int i = 0; i < cmd_count; i++) {
                char *command = commands[i];
                type_single_cmd(command);
        }

        return SHELL_CONTINUE;
}

void type_single_cmd(char *command)
{
        int found = is_builtin_cmd(command);
        if (found)
                printf("%s is a shell builtin\n", command);

        if (!found) {
                char *path = strdup(getenv("PATH"));

                if (path) {
                        char *dir = strtok(path, ":");

                        while (dir) {
                                char full_path[MAX_BUFF_SIZE];
                                snprintf(full_path, sizeof(full_path),
                                         "%s/%s", dir,
                                         command);

                                if (access(full_path, X_OK) == 0) {
                                        printf("%s is %s\n",
                                               command,
                                               full_path);
                                        found = 1;
                                        break;
                                }
                                dir = strtok(NULL, ":");
                        }
                }
                free(path);
        }

        if (!found)
                printf("%s: not found\n", command);
}

unsigned char is_builtin_cmd(char *command)
{
        for (int i = 0; builtins[i].name; i++) {
                if (strcmp(command, builtins[i].name) == 0) {
                        return 1;
                }
        }

        return 0;
}

unsigned char builtin_name_to_idx(char *command)
{
        assert(is_builtin_cmd(command));

        unsigned char count = 0;

        while (TRUE) {
                if (strcmp(builtins[count].name, command) == 0) {
                        break;
                }
                count++;
        }

        return count;
}
