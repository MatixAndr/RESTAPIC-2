//
// Created by MateuszAndruszkiewic on 3.12.2024.
//

#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>

#define MAX_REQUEST_LENGTH 8192 // Tutaj polecam dostosować dla własnego uznania bo czasami (prawie nigdy w realnych przypadkach) może nie wystarczyć
#define MAX_RESPONSE_LENGTH 16384 // Tego też można zmienić ale szczerze to się nie przyda no bo i tak w większości przypadków baza userów jest mała

typedef struct {
    char method[16];
    char path[256];
    char protocol[16];
    char body[MAX_REQUEST_LENGTH];
} HttpRequest;

typedef struct {
    int status_code;
    char status_message[64];
    char content_type[64];
    char body[MAX_RESPONSE_LENGTH];
} HttpResponse;

int parse_http_request(const char* raw_request, HttpRequest* request);
void prepare_http_response(HttpResponse* response, int status_code, const char* content_type, const char* body);
void send_http_response(SOCKET client_socket, const HttpResponse* response);
HttpResponse get_last_response();

#endif // SERVER_H

