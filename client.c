#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*libray for socket structure*/
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8000

struct sockaddr_in server;
struct sockaddr_in server_identity;

char buff[] = "Viettel High Tech";
char buff2[1024];
char IP[INET_ADDRSTRLEN];
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
    state = create_IPv4_address(&server, "192.168.27.167", PORT);
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
    state = getsockname(client_fd, (struct sockaddr *)&server_identity, &len);
    if (state < 0)
    {
        printf("Failed to get server info\n");
    }
    if (inet_ntop(AF_INET, &server_identity.sin_addr, IP, (socklen_t)INET_ADDRSTRLEN) == NULL)
    {
        printf("Failed to get server info\n");
    }
    else
    {
        printf("Server address %s\n", IP);
    }
    write(client_fd, buff, strlen(buff));
    read(client_fd, buff2, 1024);
    printf("%s", buff2);
    int val_read = 0;
    do
    {
        val_read = read(client_fd, buff2, 1024);
    } while (val_read > 0);

    close(client_fd);

    return 0;
}