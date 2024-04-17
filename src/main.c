/*
// Created by Wouter Jacobs.
*/

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

#define DEFAULT_PORT "8081"
#define DEFAULT_BUFLEN 512
int main(){

    //Initializes the Winsock library for network communication.
    int action_result;
    WSADATA wsa_data;

    action_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (action_result != 0) {
        printf("WSAStartup failed: %d\n", action_result);
        return 1;
    }else{
        printf("Winsock2 init successful\n");
    }


    /*
     * Creation, binding and listening of Sockets.
     * using: - ipv4
     *        - TCP protocol
     * Port = 8081
     */
    struct addrinfo *address_info = NULL;
    struct addrinfo *address_info_pointer = NULL;
    struct addrinfo address_info_blueprint;

    ZeroMemory(&address_info_blueprint, sizeof (address_info_blueprint));
    {
        address_info_blueprint.ai_family = AF_INET;
        address_info_blueprint.ai_socktype = SOCK_STREAM;
        address_info_blueprint.ai_protocol = IPPROTO_TCP;
        address_info_blueprint.ai_flags = AI_PASSIVE;
    }

    action_result = getaddrinfo(NULL, DEFAULT_PORT, &address_info_blueprint, &address_info);
    if (action_result != 0) {
        printf("getaddrinfo failed: %d\n", action_result);
        WSACleanup();
        return 1;
    }else{
        printf("Address init successful\n");
    }

    SOCKET listening_socket;
    listening_socket = socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);
    if (listening_socket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(address_info);
        WSACleanup();
        return 1;
    }else{
        printf("Socket init successful\n");
    }


    action_result = bind(listening_socket, address_info->ai_addr, (int)address_info->ai_addrlen);
    if (action_result == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(address_info);
        closesocket(listening_socket);
        WSACleanup();
        return 1;
    }else{
        printf("Binding socket to address and port successful\n");
    }

    freeaddrinfo(address_info);

    action_result = listen(listening_socket, SOMAXCONN);
    if (action_result == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listening_socket);
        WSACleanup();
        return 1;
    }else{
        printf("Socket successfully listening\n");
    }

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
            bytes_sent = send(client_socket, recv_buf, action_result, 0);

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
