#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>

int main()
{
    struct pollfd pfd;
    char buffer[256];

    // Monitor standard input (file descriptor 0)
    pfd.fd = STDIN_FILENO;
    pfd.events = POLLIN; // Interested in read events

    printf("Waiting for input on stdin (fd 0). Enter some text and press Enter...\n");

    while (1)
    {
        // Call poll with a timeout of 5 seconds (5000 milliseconds)
        // A negative timeout waits indefinitely.
        int ret = poll(&pfd, 1, 5000);

        if (ret == -1)
        {
            perror("poll");
            exit(EXIT_FAILURE);
        }
        else if (ret == 0)
        {
            printf("Timeout occurred. No input for 5 seconds.\n");
        }
        else
        {
            // Check if POLLIN event occurred on stdin
            if (pfd.revents & POLLIN)
            {
                ssize_t bytes_read = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);
                if (bytes_read > 0)
                {
                    buffer[bytes_read] = '\0';
                    printf("Read %zd bytes: %s", bytes_read, buffer);
                }
                else if (bytes_read == 0)
                {
                    printf("End of input (EOF) on stdin.\n");
                    break; // Exit loop on EOF
                }
                else
                {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }

    return 0;
}