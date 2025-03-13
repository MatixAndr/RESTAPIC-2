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
int parse_http_request(const char* raw_request, HttpRequest* request) {
    int ret = sscanf(raw_request, "%15s %255s %15s", request->method, request->path, request->protocol);
    if (ret != 3)
        return 0;
    const char* body = strstr(raw_request, "\r\n\r\n");
    if (body) {
        body += 4;
        strncpy(request->body, body, MAX_REQUEST_LENGTH - 1);
        request->body[MAX_REQUEST_LENGTH - 1] = '\0';
    } else {
        request->body[0] = '\0';
    }
    return 1;
}
const char* get_status_text(int status_code) {
    switch (status_code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        default:  return "";
    }
}
void prepare_http_response(HttpResponse* response, int status_code, const char* content_type, const char* body) {
    response->status_code = status_code;
    strncpy(response->content_type, content_type, sizeof(response->content_type) - 1);
    response->content_type[sizeof(response->content_type) - 1] = '\0';
    if (body) {
        strncpy(response->body, body, MAX_RESPONSE_LENGTH - 1);
        response->body[MAX_RESPONSE_LENGTH - 1] = '\0';
        response->body_length = (int)strlen(response->body);
    } else {
        response->body[0] = '\0';
        response->body_length = 0;
    }
    const char* status_text = get_status_text(status_code);
    strncpy(response->status_message, status_text, sizeof(response->status_message) - 1);
    response->status_message[sizeof(response->status_message) - 1] = '\0';
}
void format_response(const HttpResponse* response, char* formatted_response) {
    sprintf(formatted_response,
            "HTTP/1.1 %d %s\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %d\r\n\r\n%s",
            response->status_code,
            response->status_message,
            response->content_type,
            response->body_length,
            response->body);
}

void send_http_response(SOCKET client_socket, const HttpResponse* response) {
    char formatted_response[2048];
    format_response(response, formatted_response);
#ifdef ENABLE_TESTS
    typedef struct {
        char response_buffer[MAX_RESPONSE_LENGTH];
        int response_length;
    } MockSocket;
    MockSocket* mock = (MockSocket*)client_socket;
    strncpy(mock->response_buffer, formatted_response, MAX_RESPONSE_LENGTH - 1);
    mock->response_buffer[MAX_RESPONSE_LENGTH - 1] = '\0';
    mock->response_length = (int)strlen(mock->response_buffer);
#else
    send(client_socket, formatted_response, (int)strlen(formatted_response), 0);
#endif
}

HttpResponse get_last_response() {
    HttpResponse response;
    response.status_code = 0;
    response.status_message[0] = '\0';
    response.content_type[0] = '\0';
    response.body[0] = '\0';
    response.body_length = 0;
    return response;
}
