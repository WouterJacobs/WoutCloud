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

int initialize_winsock();
int setAddressInfo(struct addrinfo **address_info_pointer);

#endif //WOUTCLOUD_MAIN_H
