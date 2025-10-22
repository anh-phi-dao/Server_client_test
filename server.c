#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*libray for socket structure*/
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8000

struct sockaddr_in server;
struct sockaddr_in identity;
char buff[1024];

void create_IPv4_server_with_any_addresses(struct sockaddr_in *addr, int port)
{
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = INADDR_ANY;
}

char buff2[] = "Tap doan cong nghiep vien thong quan doi Viettel\n";
char IP[INET_ADDRSTRLEN];
int main()
{
    int main_server_fd, new_socket;
    int state = 0;
    socklen_t len = (socklen_t)sizeof(server);

    /*create a tcp socket server with IPv4*/
    main_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Server fd is %d\n", main_server_fd);

    int opt = 1;
    state = setsockopt(main_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    create_IPv4_server_with_any_addresses(&server, PORT);
    do
    {
        state = bind(main_server_fd, (struct sockaddr *)&server, len);
        printf("Binding\n");

    } while (state < 0);
    state = getsockname(main_server_fd, (struct sockaddr *)&identity, &len);
    if (state < 0)
    {
        printf("Can not get server information\n");
    }
    if (inet_ntop(AF_INET, &identity.sin_addr, IP, (socklen_t)INET_ADDRSTRLEN) == NULL)
    {
        printf("Can not get server information\n");
    }
    printf("Server address :%s\n", IP);
    state = listen(main_server_fd, 2);
    if (state < 0)
    {
        printf("Failed on establish a server\n");
        return -1;
    }
    printf("Ready to connect \n");
    /*this function will block until client is connected*/
    new_socket = accept(main_server_fd, (struct sockaddr *)&server, &len);
    if (new_socket < 0)
    {
        printf("Error when trying to accept the client\n");
        return -1;
    }
    else
    {
        printf("The client socket has a fd which is %d\n", new_socket);
    }
    state = getpeername(new_socket, (struct sockaddr *)&identity, &len);
    if (state < 0)
    {
        printf("Can not get client information\n");
    }
    if (inet_ntop(AF_INET, &identity.sin_addr, IP, (socklen_t)INET_ADDRSTRLEN) == NULL)
    {
        printf("Can not get client information\n");
    }
    printf("client address :%s\n", IP);
    int val_read = read(new_socket, buff, 1024);
    printf("%s\n", buff);
    write(new_socket, buff2, strlen(buff2));
    close(new_socket);
    close(main_server_fd);
    return 0;
}
