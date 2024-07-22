#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#include "main.h"

void *handle_client(void *socket_desc);

int main() {
    int server_fd;
    int new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    server_fd = createServerSocket(server_fd);
    if (server_fd <= 0) {
        return -1;
    }

    setSocketOptions(server_fd);
    setAddressOptions(&address);
    bindAddressToSocket(server_fd, &address);

    if (listen(server_fd, 3) < 0) {
        error("Listen failed");
    }
    printf("Server is listening on port %d\n", PORT);

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            error("Accept failed");
        }

        pthread_t client_thread;
        int *new_sock = malloc(sizeof(int));
        *new_sock = new_socket;

        if (pthread_create(&client_thread, NULL, handle_client, (void*)new_sock) < 0) {
            error("Could not create thread");
        }

        pthread_detach(client_thread); // To automatically reclaim thread resources
    }

    close(server_fd);

    return 0;
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int createServerSocket(int server_fd) {
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        error("Socket creation failed");
        return -1;
    }
    printf("Server socket created...\n");
    return server_fd;
}

void setSocketOptions(int socket) {
    int option = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option))) {
        error("setsockopt");
    }
}

void setAddressOptions(struct sockaddr_in* address) {
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);
}

void bindAddressToSocket(int server_fd, struct sockaddr_in* address) {
    if (bind(server_fd, (struct sockaddr*) address, sizeof(*address)) < 0) {
        error("Bind failed");
    }
}

void *handle_client(void *socket_desc) {
    int sock = *(int*)socket_desc;
    free(socket_desc);
    char buffer[BUFFER_SIZE] = {0};
    const char *welcome = "Welcome to the chat server! Type 'exit' to disconnect.\n";

    send(sock, welcome, strlen(welcome), 0);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread < 0) {
            error("Read failed");
        }

        if (strncmp(buffer, "exit", 4) == 0) {
            printf("Client has disconnected\n");
            break;
        }

        printf("Client: %s\n", buffer);

        send(sock, buffer, strlen(buffer), 0);
    }

    close(sock);
    pthread_exit(NULL);

    return 0;
}
