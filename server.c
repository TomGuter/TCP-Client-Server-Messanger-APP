#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8000
#define BUFFER_SIZE 1024

void exchange_messages(int sock, const char *client_name, const char *server_name)
{
    char buffer[BUFFER_SIZE] = {0};

    while (1)
    {

        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0)
        {
            printf("Message by %s: %s", client_name, buffer);
            if (strcmp(buffer, "bye\n") == 0 || strcmp(buffer, "finish\n") == 0)
            {
                printf("%s requested to close the connection. Closing...\n", client_name);
                break;
            }
        }

        printf("Enter your message %s: ", server_name);
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sock, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "bye\n") == 0 || strcmp(buffer, "finish\n") == 0)
        {
            printf("The other side requseted to close the connection. Closing...\n");
            break;
        }
        printf("Waiting for %s answer...\n", client_name);
    }
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char client_name[BUFFER_SIZE] = {0};
    char server_name[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Enter your name: ");
    fgets(server_name, BUFFER_SIZE, stdin);
    server_name[strcspn(server_name, "\n")] = 0;
    send(new_socket, server_name, strlen(server_name), 0);

    memset(client_name, 0, BUFFER_SIZE);
    int valread = read(new_socket, client_name, BUFFER_SIZE);
    if (valread > 0)
    {
        client_name[strcspn(client_name, "\n")] = 0;
    }

    printf("Hi %s, enter your message. If you want to finish the conversation enter \"bye\" or \"finish\"\n", server_name);
    exchange_messages(new_socket, client_name, server_name);

    close(new_socket);
    close(server_fd);
    return 0;
}
