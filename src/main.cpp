#include <ws2tcpip.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"
#include "routes.h"
#include <stdio.h>
#include "user.h"
#pragma comment(lib, "ws2_32.lib")
#define PORT 8888
#define MAX_REQUEST_SIZE 4096

void run_server() {
    WSADATA wsa_data;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    char request_buffer[MAX_REQUEST_SIZE];
    int client_addr_len = sizeof(client_addr);
    int recv_size;
    printf("RESTAPIC Server - Starting...\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        return;
    }
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return;
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return;
    }
    listen(server_socket, 5);
    printf("RESTAPIC Server running on port %d...\n", PORT);
    printf("Endpoints available:\n");
    printf("  GET    /users       - Get all users\n");
    printf("  GET    /users/:id   - Get user by ID\n");
    printf("  POST   /users       - Create a new user\n");
    printf("  PATCH  /users/:id   - Update user\n");
    printf("  DELETE /users/:id   - Delete user\n");
    while (1) {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len)) == INVALID_SOCKET) {
            printf("Accept failed. Error Code: %d\n", WSAGetLastError());
            continue;
        }
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("Connection accepted from %s:%d\n", client_ip, ntohs(client_addr.sin_port));
        memset(request_buffer, 0, MAX_REQUEST_SIZE);
        if ((recv_size = recv(client_socket, request_buffer, MAX_REQUEST_SIZE, 0)) == SOCKET_ERROR) {
            printf("recv failed. Error Code: %d\n", WSAGetLastError());
            closesocket(client_socket);
            continue;
        }
        if (recv_size > 0) {
            request_buffer[recv_size] = '\0';
            handle_request(request_buffer, client_socket);
        }
        closesocket(client_socket);
    }
    closesocket(server_socket);
    WSACleanup();
}

void run_tests(const char* test_type) {
    extern void run_unit_tests();
    extern void run_integration_tests();
    printf("RESTAPIC Test Framework\n");
    printf("Version: 1.3.7\n");
    if (strcmp(test_type, "u") == 0) {
        printf("Running unit tests...\n\n");
        run_unit_tests();
    } else if (strcmp(test_type, "i") == 0) {
        printf("Running integration tests...\n\n");
        run_integration_tests();
    } else if (strcmp(test_type, "all") == 0) {
        printf("Running all tests...\n\n");
        printf("========== UNIT TESTS ==========\n\n");
        run_unit_tests();
        printf("\n\n========== INTEGRATION TESTS ==========\n\n");
        run_integration_tests();
    } else {
        printf("Unknown test type: %s\n", test_type);
        printf("Valid test types: 'u' (unit), 'i' (integration), 'all'\n");
    }
}

void print_help() {
    printf("RESTAPIC - A lightweight REST API in C++\n");
    printf("Usage:\n");
    printf("  restapic.exe                  - Start the server\n");
    printf("  restapic.exe --test <type>    - Run tests\n");
    printf("  restapic.exe --help           - Show this help message\n\n");
    printf("Test types:\n");
    printf("  u    - Run unit tests\n");
    printf("  i    - Run integration tests\n");
    printf("  all  - Run all tests\n");
}

int main(int argc, char* argv[]) {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #endif
    if (argc > 1) {
        if (strcmp(argv[1], "--test") == 0) {
            if (argc > 2) {
                run_tests(argv[2]);
            } else {
                printf("Please specify test type: --test [u|i|all]\n");
            }
        } else if (strcmp(argv[1], "--help") == 0) {
            print_help();
        } else {
            printf("Unknown option: %s\n", argv[1]);
            print_help();
        }
    } else {
        run_server();
    }
    return 0;
}
