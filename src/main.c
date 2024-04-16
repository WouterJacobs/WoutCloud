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
    int initialisation_result;
    WSADATA wsa_data;

    initialisation_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (initialisation_result != 0) {
        printf("WSAStartup failed: %d\n", initialisation_result);
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
    struct addrinfo *result = NULL, *pointer = NULL, hints;

    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    initialisation_result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (initialisation_result != 0) {
        printf("getaddrinfo failed: %d\n", initialisation_result);
        WSACleanup();
        return 1;
    }else{
        printf("Address init successful\n");
    }

    SOCKET listening_socket;
    listening_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (listening_socket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }else{
        printf("Socket init successful\n");
    }


    initialisation_result = bind(listening_socket, result->ai_addr, (int)result->ai_addrlen);
    if (initialisation_result == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listening_socket);
        WSACleanup();
        return 1;
    }else{
        printf("Binding socket to address and port successful\n");
    }

    freeaddrinfo(result);

    initialisation_result = listen(listening_socket, SOMAXCONN);
    if (initialisation_result == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listening_socket);
        WSACleanup();
        return 1;
    }else{
        printf("Socket successfully listening\n");
    }

    /*
     * Server loop
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
    int init_send_result;
    int recv_buflen = DEFAULT_BUFLEN;

    // Receive until the peer shuts down the connection
    do {
        initialisation_result = recv(client_socket, recv_buf, recv_buflen, 0);
        if (initialisation_result > 0) {
            printf("Bytes received: %d\n", initialisation_result);

            // Echo the buffer back to the sender
            init_send_result = send(client_socket, recv_buf, initialisation_result, 0);
            if (init_send_result == SOCKET_ERROR) {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(client_socket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", init_send_result);
        } else if (initialisation_result == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(client_socket);
            WSACleanup();
            return 1;
        }

    } while (initialisation_result > 0);



    printf("server successfully shut down");
    return 0;
}
