#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
        char buffer[128];

        // Flush after every printf
        setbuf(stdout, NULL);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                printf("ERROR: Could not read input command\n");
        }

        buffer[strlen(buffer) - 1] = '\0';

        // TODO: Uncomment the code below to pass the first stage
        printf("$ ");

        printf("\n%s: command not found\n", buffer);

        return 0;
}
