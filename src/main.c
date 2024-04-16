/*
// Created by Wouter Jacobs.
*/

#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>

int main(){
    int initialisation_result;
    WSADATA wsa_data;

    // Initialization

    initialisation_result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (initialisation_result != 0) {
        printf("WSAStartup failed: %d\n", initialisation_result);
        return 1;
    }else{
        printf("init succesfull");
    }


    return 0;
}
