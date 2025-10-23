#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*libray for socket structure*/
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8000

struct sockaddr_in server;

char buff[] = "Port:8000 IP:192.168.1.103 Viettel High Tech";
char buff2[1024];
int val_read;
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

    do
    {
        int buff_len = strlen(buff2);
        for (int i = 0; i < buff_len; i++)
        {
            buff2[i] = 0;
        }
        write(client_fd, buff, 45);
        val_read = (client_fd, buff2, 34);
        printf("Val read=%d\n", val_read);
        printf("%s\n", buff2);
        sleep(1);

    } while (val_read != 0);
    close(client_fd);
    printf("Client has close\n");
    return 0;
}