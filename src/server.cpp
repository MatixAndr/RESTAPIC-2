//
// Created by MateuszAndruszkiewic on 3.12.2024.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "server.h"

static HttpResponse last_response;

int parse_http_request(const char* raw_request, HttpRequest* request) {
    char* request_copy = strdup(raw_request);
    char* saveptr;
    char* line;

    line = strtok_r(request_copy, "\r\n", &saveptr);
    if (!line) {
        free(request_copy);
        return 0;
    }

    sscanf(line, "%15s %255s %15s",
           request->method,
           request->path,
           request->protocol);

    char* body_start = strstr(raw_request, "\r\n\r\n");
    if (body_start) {
        strncpy(request->body, body_start + 4, MAX_REQUEST_LENGTH - 1);
    } else {
        request->body[0] = '\0';
    }

    free(request_copy);
    return 1;
}

void prepare_http_response(HttpResponse* response, int status_code,
                           const char* content_type, const char* body) {
    response->status_code = status_code;

    switch(status_code) {
        case 200: strcpy(response->status_message, "OK"); break;
        case 201: strcpy(response->status_message, "Created"); break;
        case 204: strcpy(response->status_message, "No Content"); break;
        case 400: strcpy(response->status_message, "Bad Request"); break;
        case 404: strcpy(response->status_message, "Not Found"); break;
        case 405: strcpy(response->status_message, "Method Not Allowed"); break;
        case 413: strcpy(response->status_message, "Payload Too Large"); break;
        case 500: strcpy(response->status_message, "Internal Server Error"); break;
        default: strcpy(response->status_message, "Unknown"); break;
    }

    strncpy(response->content_type, content_type, sizeof(response->content_type) - 1);
    strncpy(response->body, body, MAX_RESPONSE_LENGTH - 1);

    // Update the last_response
    memcpy(&last_response, response, sizeof(HttpResponse));
}

void send_http_response(SOCKET client_socket, const HttpResponse* response) {
    char full_response[MAX_RESPONSE_LENGTH];

    snprintf(full_response, sizeof(full_response),
             "HTTP/1.1 %d %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n%s",
             response->status_code,
             response->status_message,
             response->content_type,
             strlen(response->body),
             response->body);

    send(client_socket, full_response, strlen(full_response), 0);
}

HttpResponse get_last_response() {
    return last_response;
}

