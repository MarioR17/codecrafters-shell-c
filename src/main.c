#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
        char buffer[128];

        while (1) {
                // Flush after every printf
                setbuf(stdout, NULL);

                printf("$ ");

                if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                        printf("ERROR: Could not read input command\n");
                }

                buffer[strlen(buffer) - 1] = '\0';

                char *command = strtok(buffer, " ");

                if (strcmp(command, "echo") == 0) {
                        printf("%s\n", (buffer + strlen(command) + 1));
                } else if (strcmp(command, "exit") == 0) {
                        break;
                } else {
                        printf("%s: command not found\n", buffer);
                }

        }

        return 0;
}
