// Header file for main

// Includes
#include <netinet/in.h>

// Definitions
#define PORT 8080
#define BUFFER_SIZE 1024

// Methods
void error(const char *msg);
int createServerSocket(int server_fd );
void setSocketOptions(int socket);
void setAddressOptions(struct sockaddr_in* address);
void bindAddressToSocket(int server_fd, struct sockaddr_in* address);
