#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/wait.h>

#include "shell.h"

// map builtin commands with their respective function
static struct builtin builtins[] = {
        {"exit", cmd_exit},
        {"echo", cmd_echo},
        {"type", cmd_type},
        {NULL, NULL}
};

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
                // execute given builtin function with given arguments
                return builtins[builtin_name_to_idx(command)].func(args);
        } else {
                char exec_path[MAX_BUFF_SIZE];

                // get absolute path of command if it exists
                unsigned char found = get_exec_command_path(exec_path,
                                              MAX_BUFF_SIZE,
                                              command);

                // if command exists from PATH
                if (found) {
                        char *commands[MAX_COMMANDS];
                        commands[0] = command;

                        if (args) {
                                // rest of array is the command arguments given
                                get_args_list(&commands[1], MAX_COMMANDS - 1,
                                              args);
                        } else {
                                commands[1] = NULL;
                        }

                        pid_t p = fork();
                        if (p < 0) {
                                perror("Forking failed\n");
                                exit(EXIT_FAILURE);
                        } else if (p > 0) {
                                int status;
                                // wait on child process to finish
                                waitpid(p, &status, 0);
                        } else {
                                // execute the command with its arguments
                                // as child process
                                execv(exec_path, commands);
                                perror("Did not exec command\n");
                                exit(EXIT_FAILURE);
                        }

                        return SHELL_CONTINUE;
                }
        }

        printf("%s: command not found\n", command);

        return SHELL_CONTINUE;
}

/*
 * exit the shell loop
*/
unsigned char cmd_exit(char *args)
{
        (void)args;
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
        int cmd_count = get_args_list(commands, MAX_COMMANDS, args);

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
        unsigned char found = is_builtin_cmd(command);
        if (found)
                printf("%s is a shell builtin\n", command);

        // executable from path
        if (!found) {
                char full_path[MAX_BUFF_SIZE]; 

                found = get_exec_command_path(full_path, MAX_BUFF_SIZE,
                                              command);

                if (found)
                        printf("%s is %s\n", command, full_path);
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
                        return FOUND;
                }
        }

        return NOT_FOUND;
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

/*
 * set the buffer to the path of the executable command if found
*/
unsigned char get_exec_command_path(char *buffer, int buff_size, char *command)
{
        // ensure the given command is not a builtin command
        assert(!is_builtin_cmd(command));
        
        // allocate string to be the value of path
        char *path = strdup(getenv("PATH"));

        if (path) {
                // iterate through each variable in path
                char *dir = strtok(path, ":");

                while (dir) {
                        // set string to "dir/command" format
                        int size_allocated = snprintf(buffer, buff_size,
                                                      "%s/%s", dir,
                                                      command);

                        // if could not allocate properly
                        if (size_allocated < 0 || size_allocated >= buff_size)
                                goto out_free_err;

                        // check if it has execution access
                        if (access(buffer, X_OK) == 0) {
                                free(path);
                                return FOUND;
                        }

                        dir = strtok(NULL, ":");
                }
        }

out_free_err:
        free(path);
        return NOT_FOUND;
}

int get_args_list(char *buffer[], int buff_size, char *args)
{
        int cmd_count = 0;
        char *cmd_token = strtok(args, " \n\t");

        // make array with each command
        while (cmd_token && cmd_count < (buff_size - 1)) {
                buffer[cmd_count++] = cmd_token;
                cmd_token = strtok(NULL, " \n\t");
        }
        buffer[cmd_count] = NULL;

        return cmd_count;
}
