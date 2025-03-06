#include "server.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>

#define MAX_BUFFER_SIZE 4096

int start_server(const char* port) {
    WSADATA wsa_data;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return 1;
    }

    struct addrinfo hints;
    struct addrinfo* server_info;
    struct addrinfo* p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    result = getaddrinfo(NULL, port, &hints, &server_info);
    if (result != 0) {
        printf("getaddrinfo failed: %d\n", result);
        WSACleanup();
        return 1;
    }

    SOCKET server_socket = INVALID_SOCKET;
    for (p = server_info; p != NULL; p = p->ai_next) {
        server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (server_socket == INVALID_SOCKET) {
            continue;
        }

        result = bind(server_socket, p->ai_addr, (int)p->ai_addrlen);
        if (result == SOCKET_ERROR) {
            closesocket(server_socket);
            server_socket = INVALID_SOCKET;
            continue;
        }

        break;
    }

    freeaddrinfo(server_info);

    if (server_socket == INVALID_SOCKET) {
        printf("Unable to bind to port %s\n", port);
        WSACleanup();
        return 1;
    }

    result = listen(server_socket, SOMAXCONN);
    if (result == SOCKET_ERROR) {
        printf("listen failed: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %s\n", port);

    SOCKET client_socket;
    struct sockaddr_storage client_address;
    socklen_t client_address_len = sizeof(client_address);
    char buffer[MAX_BUFFER_SIZE];

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_address_len);
        if (client_socket == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            continue;
        }

        int bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("Received: %s\n", buffer);

            const char* response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello, World!";
            send(client_socket, response, (int)strlen(response), 0);
        }

        closesocket(client_socket);
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}