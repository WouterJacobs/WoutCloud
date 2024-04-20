/*
// Created by Wouter Jacobs.
*/

#include "main.h"
unsigned __stdcall ReceiveMessages(void* socket) {
    SOCKET ConnectSocket = *((SOCKET*)socket);
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    int iResult;
    do {
        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            printf("Client: %s\n", recvbuf);
        } else if (iResult == 0) {
            printf("Server closed the connection\n");
        } else {
            printf("recv failed with error: %d\n", WSAGetLastError());
        }
    } while (iResult > 0);

    return 0;
}

int main() {
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    setTextColorGreen(hConsole);

    if (!initialize_winsock()) return 1;

    /*
     * Creation, binding and listening of Sockets.
     * using: - ipv4
     *        - TCP protocol
     * Port = 8081
     */
    struct addrinfo address_info;
    struct addrinfo *address_info_pointer = &address_info;
    if (!setAddressInfo(&address_info_pointer)) return 1;

    SOCKET listening_socket = createSocket(address_info_pointer);
    if (listening_socket == INVALID_SOCKET) return 1;

    if (!bindSocket(listening_socket, address_info_pointer))return 1;

    freeaddrinfo(address_info_pointer);

    if (!setSocketToListen(listening_socket)) return 1;

    /*
     * Server loop.
     */


    SOCKET client_socket;
    char message[DEFAULT_BUFLEN];
    if (!acceptClient(listening_socket, &client_socket)) return 1;
    // Receive and send messages
    HANDLE receiveThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ReceiveMessages, (LPVOID)&client_socket, 0, NULL);    // Main loop for sending messages
    while (1) {
        // Send message to server
        printf("You: ");
        fgets(message, sizeof(message),stdin);
        send(client_socket, message, strlen(message), 0);

        // Check if user wants to end the chat
        if (strcmp(message, "exit") == 0) {
            printf("Chat ended by user\n");
            break;
        }
    }

    // Wait for the receiving thread to finish
    WaitForSingleObject(receiveThread, INFINITE);


    /*
     * Shutting down the server.
     */
    if(!shutdownSocket(client_socket)) return 1;

    // Memory cleanup
    WSACleanup();
    printf("server successfully shut down");
    return 0;
}

byte initialize_winsock() {
    WSADATA wsa_data;
    int WSAStartup_result;

    WSAStartup_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (WSAStartup_result != 0) {
        setTextColorRed(hConsole);
        printf("WSAStartup failed: %d\n", WSAStartup_result);
        return 0;
    }
    printf("Winsock2 init successful\n");
    return 1;
}

byte setAddressInfo(struct addrinfo **address_info_pointer) {
    int action_result;
    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    action_result = getaddrinfo(NULL, DEFAULT_PORT, &hints, address_info_pointer);
    if (action_result != 0) {
        setTextColorRed(hConsole);
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
        setTextColorRed(hConsole);
        printf("Error at socket(): %d\n", WSAGetLastError());
        freeaddrinfo(address_info);
        WSACleanup();
        return INVALID_SOCKET;
    }
    printf("Socket init successful\n");
    return listening_socket;
}

byte bindSocket(SOCKET listening_socket, struct addrinfo *address_info) {
    int action_result;

    action_result = bind(listening_socket, address_info->ai_addr, (int) address_info->ai_addrlen);
    if (action_result == SOCKET_ERROR) {
        setTextColorRed(hConsole);
        printf("bind failed with error: %d\n", WSAGetLastError());
        closesocket(listening_socket);
        return 0;
    } else {
        printf("Binding socket to address and port successful\n");
        return 1;
    }
}

byte setSocketToListen(SOCKET listening_socket) {
    int action_result;

    action_result = listen(listening_socket, SOMAXCONN);
    if (action_result == SOCKET_ERROR) {
        setTextColorRed(hConsole);
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(listening_socket);
        WSACleanup();
        return 0;
    }
    setTextColorYellow(hConsole);
    printf("Socket successfully listening...\n");
    setTextColorGreen(hConsole);
    return 1;
}

byte acceptClient(SOCKET listening_socket, SOCKET *client_socket) {
    // Accepts a single client socket!
    *client_socket = accept(listening_socket, NULL, NULL);
    if ((SOCKET) client_socket == INVALID_SOCKET) {
        setTextColorRed(hConsole);
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(listening_socket);
        WSACleanup();
        return 0;
    }
    printf("Successfully accepted client connection\n");
    return 1;
}

byte handleClient(SOCKET client_socket) {
    //TODO make multithreading for incoming and outgoing messages. Split this function.
    resetTextColor(hConsole);

    if (!handleIncomingMessage(client_socket)) return 0;
    if (!handleSendingMessage(client_socket)) return 0;

    // setting the color back to green after communication is done.
    setTextColorGreen(hConsole);
    return 1;
}

byte handleSendingMessage(SOCKET client_socket){
    int send_result;
    int send_buflen = DEFAULT_BUFLEN;
    char *hello_string = "Hello to you too Client, this is WoutCloud speaking";

    send_result = send(client_socket, hello_string, send_buflen, 0); //sending

    if (send_result == SOCKET_ERROR) {
        setTextColorRed(hConsole);
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 0;
    }
    printf("You: %s\n", hello_string);
    return 1;
}

byte handleIncomingMessage(SOCKET client_socket){
    int recv_result;
    char recv_buf[DEFAULT_BUFLEN];
    int recv_buflen = DEFAULT_BUFLEN;

    recv_result = recv(client_socket, recv_buf, recv_buflen, 0); //receiving

    if (recv_result == SOCKET_ERROR) {
        setTextColorRed(hConsole);
        printf("recv failed: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 0;
    }
    printf("message from client: %s\n", recv_buf);
    return 1;
}

int getCorrectBytesToSend(char *string) {
    int extra_byte = 1;
    return (int) strlen(string) + extra_byte;
}

byte shutdownSocket(SOCKET client_socket){
    int action_result;

    action_result = shutdown(client_socket, SD_SEND);
    if (action_result == SOCKET_ERROR) {
        setTextColorRed(hConsole);
        printf("shutting down of socket failed with error: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 0;
    }
    action_result = closesocket(client_socket);
    if (action_result == SOCKET_ERROR) {
        setTextColorRed(hConsole);
        printf("closing of socket failed with error: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 0;
    }
    printf("socket terminated successfully\n");
    return 1;
}

void setTextColorRed(HANDLE hconsole){
    SetConsoleTextAttribute(hconsole,FOREGROUND_RED);
}

void setTextColorGreen(HANDLE hconsole){
    SetConsoleTextAttribute(hconsole,FOREGROUND_GREEN);
}

void setTextColorYellow(HANDLE hconsole){
    SetConsoleTextAttribute(hconsole, FOREGROUND_RED | FOREGROUND_GREEN);
}

void resetTextColor(HANDLE hconsole){
    SetConsoleTextAttribute(hconsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}