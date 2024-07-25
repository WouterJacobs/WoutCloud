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

int num_clients = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

struct user {
    char username[20];
    int client_socket;
};

struct user users[MAX_USERS];

int main() {
    int server_fd;
    int new_socket;
    struct sockaddr_in address;
    int address_lenght = sizeof(address);
    int total_pending_requests = 5;

    server_fd = create_server_socket(server_fd);
    if (server_fd <= 0) {
        return -1;
    }

    set_socket_options(server_fd);
    set_address_options(&address);
    bind_address_to_socket(server_fd, &address);
    set_socket_to_listen(server_fd, total_pending_requests);

    pthread_t command_thread;
    if (pthread_create(&command_thread, NULL, handle_server_commands, NULL) < 0) {
        error("Could not create server command thread");
    }

    while (1) {
        new_socket = accept(server_fd, (struct sockaddr *) &address, (socklen_t * ) & address_lenght);
        if (new_socket < 0) {
            error("Accept failed");
        }

        pthread_t client_thread;
        int *new_sock = malloc(sizeof(int));
        *new_sock = new_socket;

        if (pthread_create(&client_thread, NULL, handle_client, (void *) new_sock) < 0) {
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

int create_server_socket(int server_fd) {
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        error("Socket creation failed");
        return -1;
    }
    printf("Server socket created...\n");
    return server_fd;
}

void set_socket_options(int socket) {
    int option = 1;
    if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option))) {
        error("setsockopt");
    }
}

void set_address_options(struct sockaddr_in *address) {
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);
}

void bind_address_to_socket(int server_fd, struct sockaddr_in *address) {
    if (bind(server_fd, (struct sockaddr *) address, sizeof(*address)) < 0) {
        error("Bind failed");
    }
}

void set_socket_to_listen(int server_fd, int total_pending_requests) {
    if (listen(server_fd, 3) < 0) {
        error("Listen failed");
    }
    printf("Server is listening on port %d\n", PORT);
}

void *handle_client(void *socket_desc) {
    struct user client_user;

    int sock = *(int *) socket_desc;
    client_user.client_socket = sock;

    pthread_mutex_lock(&clients_mutex);
    users[num_clients++] = client_user;
    pthread_mutex_unlock(&clients_mutex);

    free(socket_desc);

    char buffer[BUFFER_SIZE] = {0};
    const char *welcome = "Welcome to the chat server! Type 'exit' to disconnect.\nPlease enter your username: ";

    send(sock, welcome, strlen(welcome), 0);

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);

        int valread = read(sock, buffer, BUFFER_SIZE);
        if (valread < 0) {
            perror("Read failed");
            close(sock);
            return NULL;
        }

        if (buffer[valread - 1] == '\n') {
            buffer[valread - 1] = '\0';
        }

        if (set_username(buffer, &client_user) == 1) {
            printf("Username set successfully: %s\n", client_user.username);
            break;
        } else {
            const char *error_msg = "Failed to set username. Username too long. Please try again: ";
            send(sock, error_msg, strlen(error_msg), 0);
        }
    }

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

        printf("%s: %s\n", client_user.username, buffer);

        broadcast_message(client_user.username, buffer, sock);
    }

    close(sock);

    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < num_clients; i++) {
        if (users[i].client_socket == sock) {
            for (int j = i; j < num_clients - 1; j++) {
                users[j].client_socket = users[j + 1].client_socket;
            }
            num_clients--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    pthread_exit(NULL);

    return 0;
}

void broadcast_message(const char *sender, const char *message, int sender_sock) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < num_clients; i++) {
        if (users[i].client_socket != sender_sock) {
            int combined_len = strlen(sender) + strlen(message) + 2; // +2 for colon and null terminator

            char *combined_message = (char *) malloc(combined_len);

            strcpy(combined_message, sender);
            strcat(combined_message, ": ");
            strcat(combined_message, message);

            if (send(users[i].client_socket, combined_message, combined_len, 0) < 0) {
                perror("Broadcast failed");
            }

            free(combined_message);
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}

int set_username(char *username, struct user *user) {
    if (strlen(username) > 20) {
        return -1;
    }

    strcpy(user->username, username);
    return 1;
}

void *handle_server_commands(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);
        if (strncmp(buffer, "shutdown", 8) == 0) {
            const char *shutdown_message = "Server is shutting down. Goodbye!\n";
            broadcast_server_message(shutdown_message);
            exit(0);
        } else {
            broadcast_server_message(buffer);
        }
    }
}

void broadcast_server_message(const char *message) {
    pthread_mutex_lock(&clients_mutex);

    for (int i = 0; i < num_clients; i++) {
        if (send(users[i].client_socket, message, strlen(message), 0) < 0) {
            perror("Server broadcast failed");
        }
    }

    pthread_mutex_unlock(&clients_mutex);
}


