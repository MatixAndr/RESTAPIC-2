//
// Created by MateuszAndruszkiewic on 3.12.2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include "json.hpp"
#include "routes.h"
#include "server.h"
#include "user.h"

using json = nlohmann::json;

void handle_request(const char* raw_request, SOCKET client_socket) {
    HttpRequest request;
    HttpResponse response;

    if (!parse_http_request(raw_request, &request)) {
        prepare_http_response(&response, 400, "application/json",
                              "{\"error\": \"Invalid request\"}");
        send_http_response(client_socket, &response);
        return;
    }

    if (strcmp(request.method, "GET") == 0) {
        if (strcmp(request.path, "/users") == 0) {
            handle_get_users(client_socket);
        } else if (strncmp(request.path, "/users/", 7) == 0) {
            int user_id = atoi(request.path + 7);
            handle_get_user_by_id(user_id, client_socket);
        }
    } else if (strcmp(request.method, "POST") == 0 &&
               strcmp(request.path, "/users") == 0) {
        handle_post_user(request.body, client_socket);
    } else if (strcmp(request.method, "PATCH") == 0 &&
               strncmp(request.path, "/users/", 7) == 0) {
        int user_id = atoi(request.path + 7);
        handle_patch_user(user_id, request.body, client_socket);
    } else if (strcmp(request.method, "PUT") == 0 &&
               strncmp(request.path, "/users/", 7) == 0) {
        int user_id = atoi(request.path + 7);
        handle_put_user(user_id, request.body, client_socket);
    } else if (strcmp(request.method, "DELETE") == 0 &&
               strncmp(request.path, "/users/", 7) == 0) {
        int user_id = atoi(request.path + 7);
        handle_delete_user(user_id, client_socket);
    }
}

void handle_get_users(SOCKET client_socket) {
    int count;
    User* users = get_all_users(&count);
    HttpResponse response;

    json json_array = json::array();
    for (int i = 0; i < count; i++) {
        json user_obj = {
            {"id", users[i].id},
            {"name", users[i].name},
            {"lastname", users[i].lastname}
        };
        json_array.push_back(user_obj);
    }

    std::string json_str = json_array.dump();
    prepare_http_response(&response, 200, "application/json", json_str.c_str());
    send_http_response(client_socket, &response);
}

void handle_get_user_by_id(int user_id, SOCKET client_socket) {
    HttpResponse response;
    User* user = get_user_by_id(user_id);

    if (user) {
        json user_obj = {
            {"id", user->id},
            {"name", user->name},
            {"lastname", user->lastname}
        };

        std::string json_str = user_obj.dump();
        prepare_http_response(&response, 200, "application/json", json_str.c_str());
    } else {
        prepare_http_response(&response, 400, "application/json",
                              "{\"error\": \"User not found\"}");
    }

    send_http_response(client_socket, &response);
}

void handle_post_user(const char* body, SOCKET client_socket) {
    HttpResponse response;
    json parsed_json;

    try {
        parsed_json = json::parse(body);
    } catch (json::parse_error& e) {
        prepare_http_response(&response, 400, "application/json",
                              "{\"error\": \"Invalid request body\"}");
        send_http_response(client_socket, &response);
        return;
    }

    if (parsed_json.contains("name") && parsed_json.contains("lastname")) {
        const std::string& name = parsed_json["name"];
        const std::string& lastname = parsed_json["lastname"];

        int new_id = add_user(name.c_str(), lastname.c_str());

        json response_body = {{"id", new_id}};
        std::string response_str = response_body.dump();

        prepare_http_response(&response, 201, "application/json", response_str.c_str());
    } else {
        prepare_http_response(&response, 400, "application/json",
                              "{\"error\": \"Invalid request body\"}");
    }

    send_http_response(client_socket, &response);
}

void handle_patch_user(int user_id, const char* body, SOCKET client_socket) {
    HttpResponse response;
    json parsed_json;

    try {
        parsed_json = json::parse(body);
    } catch (json::parse_error& e) {
        prepare_http_response(&response, 400, "application/json",
                              "{\"error\": \"Invalid request body\"}");
        send_http_response(client_socket, &response);
        return;
    }

    const char* name = parsed_json.contains("name") ? parsed_json["name"].get<std::string>().c_str() : NULL;
    const char* lastname = parsed_json.contains("lastname") ? parsed_json["lastname"].get<std::string>().c_str() : NULL;

    if (update_user_partial(user_id, name, lastname)) {
        prepare_http_response(&response, 204, "application/json", "");
    } else {
        prepare_http_response(&response, 400, "application/json",
                              "{\"error\": \"User not found\"}");
    }

    send_http_response(client_socket, &response);
}

void handle_put_user(int user_id, const char* body, SOCKET client_socket) {
    HttpResponse response;
    json parsed_json;

    try {
        parsed_json = json::parse(body);
    } catch (json::parse_error& e) {
        prepare_http_response(&response, 400, "application/json",
                              "{\"error\": \"Invalid request body\"}");
        send_http_response(client_socket, &response);
        return;
    }

    if (parsed_json.contains("name") && parsed_json.contains("lastname")) {
        const std::string& name = parsed_json["name"];
        const std::string& lastname = parsed_json["lastname"];

        update_user_full(user_id, name.c_str(), lastname.c_str());
        prepare_http_response(&response, 204, "application/json", "");
    } else {
        prepare_http_response(&response, 400, "application/json",
                              "{\"error\": \"Invalid request body\"}");
    }

    send_http_response(client_socket, &response);
}

void handle_delete_user(int user_id, SOCKET client_socket) {
    HttpResponse response;

    if (delete_user(user_id)) {
        prepare_http_response(&response, 204, "application/json", "");
    } else {
        prepare_http_response(&response, 400, "application/json",
                              "{\"error\": \"User not found\"}");
    }

    send_http_response(client_socket, &response);
}