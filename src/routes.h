#ifndef ROUTES_H
#define ROUTES_H

#include <winsock2.h>
#include "server.h"

void handle_request(const char* raw_request, SOCKET client_socket);
void handle_get_users(SOCKET client_socket);
void handle_get_user_by_id(int user_id, SOCKET client_socket);
void handle_post_user(const char* body, SOCKET client_socket);
void handle_patch_user(int user_id, const char* body, SOCKET client_socket);
void handle_delete_user(int user_id, SOCKET client_socket);

#endif
