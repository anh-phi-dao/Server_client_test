#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*libray for socket structure*/
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8000
#define MAX_CLIENTS 3

struct sockaddr_in server;
char buff[1024];

void create_IPv4_server_with_any_addresses(struct sockaddr_in *addr, int port)
{
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_ANY;
}

char buff2[] = "Tap doan cong nghiep vien thong quan doi Viettel";
fd_set readfds;
fd_set writefds;
struct timeval tv;
int server_fd, new_fd, client_fd[MAX_CLIENTS];
int max_fd;

int main()
{

    int state = 0, activity = 0;
    socklen_t len = (socklen_t)sizeof(server);

    /*create a tcp socket server with IPv4*/
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Server fd is %d\n", server_fd);

    create_IPv4_server_with_any_addresses(&server, PORT);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    state = bind(server_fd, (struct sockaddr *)&server, len);
    if (state < 0)
    {
        printf("Failed on binding server socket, the state is %d\n", state);
        return -1;
    }
    state = listen(server_fd, 10);
    if (state < 0)
    {
        printf("Failed on establish a server\n");
        return -1;
    }
    printf("Ready to connect \n");
    /*check the close condition*/
    int close_message = 0;
    while (1)
    {
        for (int i = 0; i < 1024; i++)
        {
            buff[i] = 0;
        }
        /*reset fd_set*/
        FD_ZERO(&readfds);
        /*add server_fd to readfds*/
        FD_SET(server_fd, &readfds);

        max_fd = server_fd;

        /*Add clients socket*/
        for (int i = 0; i < MAX_CLIENTS; i++)
        {

            if (client_fd[i] > 0)
            {
                FD_SET(client_fd[i], &readfds);
            }

            if (client_fd[i] > max_fd)
            {
                max_fd = client_fd[i];
            }
        }
        /*wait for clients*/
        activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        /*if server has a reading event*/
        /*if server receive a new connection*/
        if (FD_ISSET(server_fd, &readfds))
        {
            int addrlen = sizeof(server);
            new_fd = accept(server_fd, (struct sockaddr *)&server, (socklen_t *)&addrlen);
            printf("New connection: %d\n", new_fd);
            /*updating new fd*/
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_fd[i] == 0)
                {
                    client_fd[i] = new_fd;
                    break;
                }
            }
        }

        /*Dealing with clients data*/
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            int sd = client_fd[i];
            /*check if the client is ready or not*/
            if (FD_ISSET(sd, &readfds))
            {

                int val_read = read(sd, buff, 1024);
                if (val_read == 0)
                {
                    /*close and reset fd*/
                    close(sd);
                    client_fd[i] = 0;
                    printf("Client disconnected\n");
                }
                else
                {
                    printf("From client: %s\n", buff);
                    write(sd, "Server has received your message\n", 34);
                }

                if (strcmp(buff, "Close") == 0)
                {
                    close_message = 1;
                }
            }
        }

        if (close_message == 1)
        {
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                write(client_fd[i], "Close", 6);
                shutdown(client_fd[i], SHUT_WR);
            }
            break;
        }
    }

    for (int i = MAX_CLIENTS - 1; i >= 0; i--)
    {
        if (client_fd[i] > 0)
        {
            close(client_fd[i]);
        }
    }
    close(server_fd);

    return 0;
}