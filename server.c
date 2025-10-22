#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*libray for socket structure*/
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080

void create_IPv4_UDP_server(struct sockaddr_in *addr, int port)
{
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_ANY;
}

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

struct sockaddr_in server, client;
char buff[1024];
char buff2[] = "Tap doan cong nghiep vien thong quan doi Viettel\n";
char IP[INET_ADDRSTRLEN];
int main()
{
    int server_fd, new_socket;
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    printf("Server fd is %d\n", server_fd);
    int state = 0;
    socklen_t len = (socklen_t)sizeof(struct sockaddr_in);

    /*create a UDP socket server with IPv4*/
    int opt = 1;
    state = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    create_IPv4_UDP_server(&server, PORT);
    create_IPv4_address(&client, "127.0.0.1", PORT);
    do
    {
        state = bind(server_fd, (struct sockaddr *)&server, len);
        printf("Binding\n");
    } while (state < 0);
    printf("Ready to connect\n");
    state = recvfrom(server_fd, buff, 1024, 0, (struct sockaddr *)&client, &len);

    if (state < 0)
    {
        printf("Error on UDP socket\n");
    }
    else if (state > 0)
    {
        printf("Receive from client:%s", buff);
    }
    int i = 0;
again:
    state = sendto(server_fd, buff2, 50, 0, (struct sockaddr *)&client, len);

    state = recvfrom(server_fd, buff, 1024, 0, (struct sockaddr *)&client, &len);

    if (strcmp(buff, "Received") == 0)
    {
        printf("Close now\n");
    }
    else if (i < 5)
    {
        i++;
        sleep(1);
        printf("Do again\n");
        goto again;
    }

    close(server_fd);

    return 0;
}
