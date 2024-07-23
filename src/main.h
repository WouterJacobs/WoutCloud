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
int createServerSocket(int server_fd );
void setSocketOptions(int socket);
void setAddressOptions(struct sockaddr_in* address);
void bindAddressToSocket(int server_fd, struct sockaddr_in* address);
void setSocketToListen(int server_fd, int totalPendingRequests);

void *handle_client(void *socket_desc);
void broadcast_message(const char* sender, const char* message, int sender_sock);
void *handle_server_commands(void* arg);
void broadcast_server_message(const char* message);

int setUsername(char* username, struct user* user);
