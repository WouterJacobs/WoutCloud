/*
   Created by Wouter Jacobs.
   THIS IS THE LINUX VERSION OF WOUTCLOUD.

   Licensed under: GNU GENERAL PUBLIC LICENSE
                    Version 3, 29 June 2007
*/

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>

#include "main.h"

int clients[MAX_CLIENTS];
int num_clients = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

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

        pthread_mutex_lock(&clients_mutex);
        clients[num_clients++] = new_socket;
        pthread_mutex_unlock(&clients_mutex);

        if (pthread_create(&client_thread, NULL, handle_client, (void*)new_sock) < 0) {
            error("Could not create thread");
        }

        pthread_detach(client_thread);
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

        broadcast_message(buffer, sock);
    }

    close(sock);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < num_clients; i++) {
        if (clients[i] == sock) {
            for (int j = i; j < num_clients - 1; j++) {
                clients[j] = clients[j + 1];
            }
            num_clients--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    pthread_exit(NULL);

    return 0;
}

void broadcast_message(const char *message, int sender_sock) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < num_clients; i++) {
        if (clients[i] != sender_sock) {
            if (send(clients[i], message, strlen(message), 0) < 0) {
                error("Broadcast failed");
            }
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}
