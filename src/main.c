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

// builtin commands
struct builtin {
        char *name;
        unsigned char (*func)(char *args);
};

// map builtin commands with their respective function
static struct builtin builtins[] = {
        {"exit", cmd_exit},
        {"echo", cmd_echo},
        {"type", cmd_type},
        {NULL, NULL}
};

int main(int argc, char *argv[])
{
        // flush after every printf
        setbuf(stdout, NULL);

        char buffer[MAX_BUFF_SIZE];

        while (TRUE) {
                printf("$ ");

                if (!fgets(buffer, sizeof(buffer), stdin))
                        return 1;
                // get rid of newline characters in the input buffer
                buffer[strcspn(buffer, "\n")] = '\0';

                unsigned char status = process_command(buffer);

                if (status == SHELL_EXIT)
                        break;
        }

        return 0;
}

/*
 * take in input from the user and execute the command if valid
*/
unsigned char process_command(char *buffer)
{
        // first word in input
        char *command = strtok(buffer, " \n\t");

        if (!command)
                return SHELL_CONTINUE;

        // rest of input
        char *args = strtok(NULL, "");

        if (is_builtin_cmd(command)) {
                // execute builtin function with given arguments
                return builtins[builtin_name_to_idx(command)].func(args);
        }

        printf("%s: command not found\n", command);
        return SHELL_CONTINUE;
}

/*
 * exit the shell loop
*/
unsigned char cmd_exit(char *args)
{
        return SHELL_EXIT;
}

/*
 * print out the rest of the input
*/
unsigned char cmd_echo(char *args)
{
        if (args)
                printf("%s\n", args);

        return SHELL_CONTINUE;
}

/*
 * print out the type of each command given 
*/
unsigned char cmd_type(char *args)
{
        char *commands[MAX_COMMANDS];
        int cmd_count = 0;
        char *cmd_token = strtok(args, " \n\t");

        // make array with each command
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

/*
 * process the command and print if its builtin, executable, or neither 
*/
void type_single_cmd(char *command)
{
        // builtin
        int found = is_builtin_cmd(command);
        if (found)
                printf("%s is a shell builtin\n", command);

        // executable from path
        if (!found) {
                // allocate string to be the value of path
                char *path = strdup(getenv("PATH"));

                if (path) {
                        // iterate through each variable in path
                        char *dir = strtok(path, ":");

                        while (dir) {
                                char full_path[MAX_BUFF_SIZE];
                                // set string to "dir/command" format
                                snprintf(full_path, sizeof(full_path),
                                         "%s/%s", dir,
                                         command);

                                // check if it has execution access
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

        // not builtin or executable command from path
        if (!found)
                printf("%s: not found\n", command);
}

/*
 * check if given command is a part of our builtin commands list
*/
unsigned char is_builtin_cmd(char *command)
{
        for (int i = 0; builtins[i].name; i++) {
                if (strcmp(command, builtins[i].name) == 0) {
                        return 1;
                }
        }

        return 0;
}

/*
 * given a builtin command get its index in the array
*/
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
