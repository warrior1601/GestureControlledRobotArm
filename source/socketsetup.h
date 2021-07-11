#ifndef SOCKETSETUP_H
#define SOCKETSETUP_H

#include <iostream>

using namespace std;

#define _WIN32_WINNT 0x600
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
/* headers define the Windows sockets functions (WinSock) */
#include <winsock2.h>
#include <ws2tcpip.h>
/* standard windows header  */
#include <windows.h>
#include "handformat.h"

#pragma comment(lib, "Ws2_32.lib")

#define PORT "1948"

SOCKET dataSock; /* socket used to handle the connection when it is made */

void socketSetup()
{
    WSADATA wsaData; /* data structure will be filled with information about the version on Winsock on this PC */

    struct addrinfo addrHints;  /* used to tell Windows what we want to connect to */
    struct addrinfo *addrFound; /* pointer to structures filled by windows with details of possible connections */

    int i;             /* for storing error codes */
    char ipv4Buffer[64];

    // cout << "Enter remote IP address: " << endl; /*THe user inputs the IP address of the Raspberry Pi that controls the Robot Arm */
    // cin >> ipv4Buffer;
    ipv4Buffer[0] = '1';
    ipv4Buffer[1] = '9';
    ipv4Buffer[2] = '2';
    ipv4Buffer[3] = '.';
    ipv4Buffer[4] = '1';
    ipv4Buffer[5] = '6';
    ipv4Buffer[6] = '8';
    ipv4Buffer[7] = '.';
    ipv4Buffer[8] = '1';
    ipv4Buffer[9] = '.';
    ipv4Buffer[10] = '2';
    ipv4Buffer[11] = '0';
    ipv4Buffer[12] = '7';
    ipv4Buffer[13] = '\0'; //Need to ensure the address is terminated

    /* 0. Initialise Winsock */
    cout << "\nInitialising Windows sockets..." << endl;

    if ((i = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
    {
        cout << "WSAStartup failed with error: " << i << endl;
        //return 1;
    }

    /* 1. Ask Windows to initialise a structure that defines the address used for the connection */
    std::cout << "Setting up connection & address settings..." << std::endl;
    memset(&addrHints, 0x00, sizeof(addrHints)); /* set all variables in the addrInfo structure to zero */

    /* specify what you would like the connection to be */
    addrHints.ai_family = AF_INET;       /* You want an connection compatible with old-style IPV4 IP address family */
    addrHints.ai_socktype = SOCK_STREAM; /* this means you want a TCP compatible socket, SOCK_DGRAM would specify a UDP compatible socket */
    addrHints.ai_protocol = IPPROTO_TCP; /* use the TCP protocol */

    /* Ask windows to setup the connection */
    //cout <<" Address: "<< ipv4Buffer << "Port: " << PORT <<endl;
    if ((i = getaddrinfo(ipv4Buffer, PORT, &addrHints, &addrFound)) != 0)
    {
        cout << "getaddrinfo failed with error: " << endl;
        WSACleanup();
        exit(1);
    }

    /* 2. Create the outgoing socket and connect to the remote application */

    cout << "Creating a socket and connecting..." << endl;

    /* windows may have found a few possible connections, try to use each in turn until one works */
    do
    {
        /* create socket */
        if ((dataSock = socket(addrFound->ai_family, addrFound->ai_socktype, addrFound->ai_protocol)) == SOCKET_ERROR)
        {
            cout << "socket creation failed (error = " << WSAGetLastError() << ")" << endl;
            WSACleanup();
            exit(1);
        }

        /* try to connect to remote address */
        i = connect(dataSock, addrFound->ai_addr, (int)addrFound->ai_addrlen);

    } while (i == SOCKET_ERROR && (addrFound = addrFound->ai_next)); /* if connection failed, try next connection */

    if (i == SOCKET_ERROR)
    {
        /* tried all available addresses and no success */
        cout << "Unable to connect to " << ipv4Buffer<<" Port: " << PORT << endl;
        WSACleanup();
        exit(1);
    }

    cout << "Connected....." << endl;
}

void transmit(char* handID) /* 4kB buffer for data to transmit, this might be reduced */
{
        char* buffer = handID;

        int i;             /* for storing error codes */
        if ((i = send(dataSock, buffer, (int)strlen(buffer) + 1, 0)) == SOCKET_ERROR)
        {
            cout << "send failed with error: " << WSAGetLastError() << endl;
            closesocket(dataSock);
            WSACleanup();
            //return 1;
        }
        /*
         * This just enables to check what we are sending
         */
        //cout << buffer << endl;
}

#endif // SOCKETSETUP_H
