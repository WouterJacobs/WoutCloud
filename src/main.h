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

int initialize_winsock();
int setAddressInfo(struct addrinfo **address_info_pointer);
SOCKET createSocket(struct addrinfo *address_info);
int bindSocket(SOCKET listening_socket, struct addrinfo *address_info);
int setSocketToListen(SOCKET listening_socket);
int acceptClient(SOCKET listening_socket, SOCKET *client_socket);
int handleClient(SOCKET client_socket);
int getCorrectBytesToSend(char* string);
int shutdownSocket(SOCKET client_socket);


void setTextColorRed(HANDLE hconsole);
void setTextColorGreen(HANDLE hconsole);
void resetTextColor(HANDLE hconsole);

#endif //WOUTCLOUD_MAIN_H
