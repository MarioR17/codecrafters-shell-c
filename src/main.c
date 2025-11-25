#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shell.h"

int main(void)
{
        // flush after every printf
        setbuf(stdout, NULL);

        char buffer[MAX_BUFF_SIZE];

        while (TRUE) {
                printf("$ ");

                if (!fgets(buffer, sizeof(buffer), stdin))
                        return EXIT_SUCCESS;
                // get rid of newline characters in the input buffer
                buffer[strcspn(buffer, "\n")] = '\0';

                unsigned char status = process_command(buffer);

                if (status == SHELL_EXIT)
                        break;
        }

        return EXIT_SUCCESS;
}
