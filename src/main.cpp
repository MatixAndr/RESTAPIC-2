#include <winsock2.h>
#include <stdio.h>
#include <string.h>
#include <thread>
#include "routes.h"

extern int run_unit_tests();
extern int run_integration_tests();

void start_tests(const char* test_type) {
    if (strcmp(test_type, "u") == 0) {
        run_unit_tests();
    } else if (strcmp(test_type, "i") == 0) {
        run_integration_tests();
    } else if (strcmp(test_type, "all") == 0) {
    	run_unit_tests();
        run_integration_tests();
    }else {
        printf("Invalid test type. Use 'u' for unit tests or 'i' for integration tests.\n");
    }
}

int main(int argc, char* argv[]) {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int c;
    char client_message[MAX_REQUEST_LENGTH];

    // Initialize Winsock
    printf("\nInitialising Winsock...");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return 1;
    }

    printf("Initialised.\n");

    // Create a socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
        return 1;
    }

    printf("Socket created.\n");

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    // Bind
    if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d", WSAGetLastError());
        return 1;
    }

    puts("Bind done");

    // Listen to incoming connections
    listen(server_socket, 3);

    // Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);

    if (argc == 3 && strcmp(argv[1], "--test") == 0) {
        std::thread test_thread(start_tests, argv[2]);
        test_thread.detach();
    }

    while ((client_socket = accept(server_socket, (struct sockaddr*)&client, &c)) != INVALID_SOCKET) {
        puts("Connection accepted");

        // Receive a message from client
        int read_size;
        while ((read_size = recv(client_socket, client_message, MAX_REQUEST_LENGTH, 0)) > 0) {
            client_message[read_size] = '\0';
            handle_request(client_message, client_socket);
        }

        if (read_size == 0) {
            puts("Client disconnected");
        } else if (read_size == -1) {
            printf("recv failed with error code : %d", WSAGetLastError());
        }
    }

    if (client_socket == INVALID_SOCKET) {
        printf("accept failed with error code : %d", WSAGetLastError());
        return 1;
    }

    closesocket(server_socket);
    WSACleanup();

    return 0;
}