//
// Created by Wouter on 16/04/2024.
//

#ifndef WOUTCLOUD_MAIN_H
#define WOUTCLOUD_MAIN_H

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>


#define DEFAULT_PORT "8081"
#define DEFAULT_BUFLEN 512

HANDLE hConsole;

byte initialize_winsock();
byte setAddressInfo(struct addrinfo **address_info_pointer);
SOCKET createSocket(struct addrinfo *address_info);
byte bindSocket(SOCKET listening_socket, struct addrinfo *address_info);
byte setSocketToListen(SOCKET listening_socket);
byte acceptClient(SOCKET listening_socket, SOCKET *client_socket);
byte handleClient(SOCKET client_socket);
byte handleSendingMessage(SOCKET client_socket);
byte handleIncomingMessage(SOCKET client_socket);
int getCorrectBytesToSend(char* string);
byte shutdownSocket(SOCKET client_socket);

void setTextColorRed(HANDLE hconsole);
void setTextColorGreen(HANDLE hconsole);
void setTextColorYellow(HANDLE hconsole);
void resetTextColor(HANDLE hconsole);

#endif //WOUTCLOUD_MAIN_H
