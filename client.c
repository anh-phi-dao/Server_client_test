#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*libray for socket structure*/
#include <arpa/inet.h>
#include <sys/socket.h>
#include "socket.h"
#include <unistd.h>

#define PORT 8000

struct sockaddr_in server;

char buff[] = "Viettel High Tech";
char buff2[1024];
int create_IPv4_address(struct sockaddr_in *addr, char *ip_address, int port)
{
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    if (inet_pton(AF_INET, ip_address, &addr->sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    return 0;
}

int main()
{
    int client_fd, state = 0, valread;
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    socklen_t len = (socklen_t)sizeof(server);
    if (client_fd < 0)
    {
        printf("Failed on initializing client socket\n");
        return -1;
    }
    printf("client fd is %d\n", client_fd);
    state = create_IPv4_address(&server, "127.0.0.1", PORT);
    if (state < 0)
    {
        printf("Failed on creating addrees for server\n");
        return -1;
    }

    state = connect(client_fd, (struct sockaddr *)&server, len);
    if (state < 0)
    {
        printf("Client has connected failed \n");
        return -1;
    }

    write(client_fd, buff, strlen(buff));
    read(client_fd, buff2, 1024);
    printf("%s\n", buff2);
    close(client_fd);

    return 0;
}