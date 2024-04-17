/*
// Created by Wouter Jacobs.
*/

#include "main.h"

int main(){
    int action_result;

    if(!initialize_winsock())return 1;

    /*
     * Creation, binding and listening of Sockets.
     * using: - ipv4
     *        - TCP protocol
     * Port = 8081
     */
    struct addrinfo address_info;
    struct addrinfo *address_info_pointer = &address_info;
    if(!setAddressInfo(&address_info_pointer)) return 1;

    SOCKET listening_socket = createSocket(address_info_pointer);
    if (listening_socket == INVALID_SOCKET) return 1;

    if (!bindSocket(listening_socket, address_info_pointer))return 1;

    freeaddrinfo(address_info_pointer);

    if (!setSocketToListen(listening_socket)) return 1;

    /*
     * Server loop.
     */
    SOCKET client_socket;
    // Accept a single client socket!
    client_socket = accept(listening_socket, NULL, NULL);
    if (client_socket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(listening_socket);
        WSACleanup();
        return 1;
    }else{
        printf("Successfully accepted client connection\n");
    }

    char recv_buf[DEFAULT_BUFLEN];
    int bytes_sent;
    int recv_buflen = DEFAULT_BUFLEN;

    do {
        action_result = recv(client_socket, recv_buf, recv_buflen, 0);
        if (action_result > 0) {
            printf("Bytes received: %d\n", action_result);
            printf("Message: %s", recv_buf);

            const char *hello_string = "Hello to you too Client, this is WoutCloud speaking\n";
            bytes_sent = send(client_socket, hello_string, (int)strlen(hello_string) + 1, 0);

            if (bytes_sent == SOCKET_ERROR) {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(client_socket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", bytes_sent);
        } else if (action_result == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(client_socket);
            WSACleanup();
            return 1;
        }

    } while (action_result > 0);

    /*
     * Shutting down the server.
     */
    action_result = shutdown(client_socket, SD_SEND);
    if (action_result == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    // Memory cleanup
    closesocket(client_socket);
    WSACleanup();

    printf("server successfully shut down");
    return 0;
}

int initialize_winsock(){
    WSADATA wsa_data;
    int WSAStartup_result;
    WSAStartup_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (WSAStartup_result != 0) {
        printf("WSAStartup failed: %d\n", WSAStartup_result);
        return 0;
    }
    printf("Winsock2 init successful\n");
    return 1;

}

int setAddressInfo(struct addrinfo **address_info_pointer) {
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    int action_result;
    action_result = getaddrinfo(NULL, DEFAULT_PORT, &hints, address_info_pointer);
    if (action_result != 0) {
        printf("getaddrinfo failed: %d\n", action_result);
        WSACleanup();
        return 0;
    }

    printf("Address init successful\n");
    return 1;
}

SOCKET createSocket(struct addrinfo *address_info) {
    SOCKET listening_socket;
    listening_socket = socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);
    if (listening_socket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(address_info);
        WSACleanup();
        return INVALID_SOCKET;
    }
    printf("Socket init successful\n");
    return listening_socket;
}

int bindSocket(SOCKET listening_socket, struct addrinfo *address_info) {
    int action_result;
    action_result = bind(listening_socket, address_info->ai_addr, (int)address_info->ai_addrlen);
    if (action_result == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        closesocket(listening_socket);
        return 0;
    } else {
        printf("Binding socket to address and port successful\n");
        return 1;
    }
}

int setSocketToListen(SOCKET listening_socket){
    int action_result;
    action_result = listen(listening_socket, SOMAXCONN);
    if (action_result == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listening_socket);
        WSACleanup();
        return 0;
    }
    printf("Socket successfully listening\n");
    return 1;
}