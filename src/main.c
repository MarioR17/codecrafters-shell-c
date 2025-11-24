#include <stdio.h>
#include <string.h>

#define SHELL_EXIT 0
#define SHELL_CONTINUE 1

unsigned char process_command(char *buffer);
unsigned char cmd_exit(char *args);
unsigned char cmd_echo(char *args);
unsigned char cmd_type(char *args);

struct builtin {
        char *name;
        unsigned char (*func)(char *args);
};

struct builtin builtins[] = {
        {"exit", cmd_exit},
        {"echo", cmd_echo},
        {"type", cmd_type},
        {NULL, NULL}
};

int main(int argc, char *argv[])
{
        // Flush after every printf
        setbuf(stdout, NULL);

        char buffer[128];

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
        char *command_to_check = strtok(args, " \n\t");

        while (command_to_check) {
                int found = 0;
                for (int i = 0; builtins[i].name; i++) {
                        if (strcmp(command_to_check, builtins[i].name) == 0) {
                                printf("%s is a shell builtin\n", command_to_check);
                                found = 1;
                                break;
                        }
                }

                if (!found)
                        printf("%s: not found\n", command_to_check);

                command_to_check = strtok(NULL, " \n\t");
        }

        return SHELL_CONTINUE;
}
