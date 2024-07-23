// Header file for main

// Includes
#include <netinet/in.h>

// Definitions
#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_USERS 10

//structs
struct user;

// Methods
void error(const char *msg);
int create_server_socket(int server_fd );
void set_socket_options(int socket);
void set_address_options(struct sockaddr_in* address);
void bind_address_to_socket(int server_fd, struct sockaddr_in* address);
void set_socket_to_listen(int server_fd, int total_pending_requests);

void *handle_client(void *socket_desc);
void broadcast_message(const char* sender, const char* message, int sender_sock);
void *handle_server_commands(void* arg);
void broadcast_server_message(const char* message);

int set_username(char* username, struct user* user);
