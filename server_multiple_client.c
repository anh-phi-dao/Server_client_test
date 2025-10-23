#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*libray for socket structure*/
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8000
#define MAX_CLIENTS 4

void create_IPv4_server_with_any_addresses(struct sockaddr_in *addr, int port)
{
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_ANY;
}

int Handling_message(struct pollfd *fds, int *client_fd, char *buff)
{
    int close_mess = 0;
    int ret = poll(fds, 1, 0.5);
    if (ret > 0)
    {
        if (fds->revents & POLLIN)
        {
            int val_read = read(fds->fd, buff, 1024);
            if (val_read == 0)
            {
                close(fds->fd);
                *client_fd = 0;
                fds->fd = 0;
                printf("Client disconnected\n");
            }
            else
            {
                printf("From client: %s\n", buff);
                write(*client_fd, "Server has received your message\n", 34);
            }
            /*if we receive a "Close" command*/
            if (strcmp(buff, "Close") == 0)
            {
                close_mess = 1;
            }
        }
    }
    return close_mess;
}

struct pollfd read_fds[MAX_CLIENTS];
struct pollfd connect_fd;
struct sockaddr_in server;
char buff[1024];
int server_fd, new_fd, client_fd[MAX_CLIENTS];
int max_fd;
int ret;

int main()
{

    int state = 0, activity = 0;
    socklen_t len = (socklen_t)sizeof(server);

    /*create a tcp socket server with IPv4*/
    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    connect_fd.fd = server_fd;
    connect_fd.events = POLLIN;

    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        read_fds[i].events = POLLIN;
    }

    printf("Server fd is %d\n", server_fd);
    create_IPv4_server_with_any_addresses(&server, PORT);
    /* Aloow to reuse address and port*/
    int opt = 1;
    state = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (state < 0)
    {
        printf("Failed on trying to reuse address, the state is %d\n", state);
        return -1;
    }
    /*binding the server*/
    state = bind(server_fd, (struct sockaddr *)&server, len);
    if (state < 0)
    {
        printf("Failed on binding server socket, the state is %d\n", state);
        return -1;
    }
    /*Maximun clients is 10 for this server, we only use 3*/
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

        max_fd = server_fd;

        ret = poll(&connect_fd, 1, 1);

        if (ret > 0)
        {
            if ((connect_fd.revents & POLLIN) == 1)
            {
                new_fd = accept(server_fd, (struct sockaddr *)&server, &len);
                printf("New connection: %d\n", new_fd);

                for (int i = 0; i < MAX_CLIENTS; i++)
                {
                    if (client_fd[i] == 0)
                    {
                        client_fd[i] = new_fd;
                        read_fds[i].fd = new_fd;
                        break;
                    }
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (read_fds[i].fd > 2)
            {
                close_message = Handling_message(read_fds + i, client_fd + i, buff);
            }
        }

        if (close_message == 1)
        {
            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (client_fd[i] > 0)
                {
                    close(client_fd[i]);
                }
            }
            break;
        }
    }

    /*Close the server socket*/
    close(server_fd);

    return 0;
}