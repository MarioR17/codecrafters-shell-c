#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFF_SIZE 128
#define MAX_COMMANDS 64
#define SHELL_EXIT 0
#define SHELL_CONTINUE 1

unsigned char process_command(char *buffer);
unsigned char cmd_exit(char *args);
unsigned char cmd_echo(char *args);
unsigned char cmd_type(char *args);
void type_single_cmd(char *command);

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

        for (int i = 0; builtins[i].name; i++) {
                if (strcmp(command, builtins[i].name) == 0) {
                        return builtins[i].func(args);
                }
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
        int found = 0;

        for (int i = 0; builtins[i].name; i++) {
                if (strcmp(command, builtins[i].name) == 0) {
                        printf("%s is a shell builtin\n",
                               command);
                        found = 1;
                        break;
                }
        }

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
