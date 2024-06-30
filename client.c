#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8000
#define BUFFER_SIZE 1024

void exchange_messages(int sock, const char *server_name, const char *client_name)
{
    char buffer[BUFFER_SIZE] = {0};

    while (1)
    {

        printf("Enter your message %s: ", client_name);
        fgets(buffer, BUFFER_SIZE, stdin);
        send(sock, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "bye\n") == 0 || strcmp(buffer, "finish\n") == 0)
        {
            printf("The other side requseted to close the connection. Closing...\n");
            break;
        }
        printf("Waiting for %s answer...\n", server_name);

        memset(buffer, 0, BUFFER_SIZE);
        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread > 0)
        {
            printf("Message by %s: %s", server_name, buffer);
            if (strcmp(buffer, "bye\n") == 0 || strcmp(buffer, "finish\n") == 0)
            {
                printf("%s requested to close the connection. Closing...\n", server_name);
                break;
            }
        }
    }
}

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    char server_name[BUFFER_SIZE] = {0};
    char client_name[BUFFER_SIZE] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("Invalid address/Address not supported");
        exit(EXIT_FAILURE);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    printf("Enter your name: ");
    fgets(client_name, BUFFER_SIZE, stdin);
    client_name[strcspn(client_name, "\n")] = 0;
    send(sock, client_name, strlen(client_name), 0);

    memset(server_name, 0, BUFFER_SIZE);
    int valread = read(sock, server_name, BUFFER_SIZE);
    if (valread > 0)
    {
        server_name[strcspn(server_name, "\n")] = 0;
    }

    printf("Hi %s, enter your message. If you want to finish the conversation enter \"bye\" or \"finish\"\n", client_name);
    exchange_messages(sock, server_name, client_name);

    close(sock);
    return 0;
}
