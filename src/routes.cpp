#include <winsock2.h>
#include <string.h>
#include <stdlib.h>
#include "json.hpp"
#include "routes.h"
#include "server.h"
#include "user.h"

using json = nlohmann::json;

void handle_request(const char* raw_request, SOCKET client_socket) {
    HttpRequest request;
    HttpResponse response;
    if (!parse_http_request(raw_request, &request)) {
        prepare_http_response(&response, 400, "application/json", "{\"error\": \"Invalid request\"}");
        send_http_response(client_socket, &response);
        return;
    }
    if (strcmp(request.method, "GET") == 0) {
        if (strcmp(request.path, "/users") == 0) {
            handle_get_users(client_socket);
        } else if (strncmp(request.path, "/users/", 7) == 0) {
            int user_id = atoi(request.path + 7);
            handle_get_user_by_id(user_id, client_socket);
        } else {
            prepare_http_response(&response, 404, "application/json", "{\"error\": \"Endpoint not found\"}");
            send_http_response(client_socket, &response);
        }
    } else if (strcmp(request.method, "POST") == 0 && strcmp(request.path, "/users") == 0) {
        handle_post_user(request.body, client_socket);
    } else if (strcmp(request.method, "PATCH") == 0 && strncmp(request.path, "/users/", 7) == 0) {
        int user_id = atoi(request.path + 7);
        handle_patch_user(user_id, request.body, client_socket);
    } else if (strcmp(request.method, "DELETE") == 0 && strncmp(request.path, "/users/", 7) == 0) {
        int user_id = atoi(request.path + 7);
        handle_delete_user(user_id, client_socket);
    } else {
        prepare_http_response(&response, 404, "application/json", "{\"error\": \"Endpoint not found\"}");
        send_http_response(client_socket, &response);
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
            {"firstName", users[i].firstName},
            {"lastName", users[i].lastName},
            {"age", calculate_age(users[i].birthYear)},
            {"group", users[i].group}
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
            {"firstName", user->firstName},
            {"lastName", user->lastName},
            {"age", calculate_age(user->birthYear)},
            {"group", user->group}
        };
        std::string json_str = user_obj.dump();
        prepare_http_response(&response, 200, "application/json", json_str.c_str());
    } else {
        prepare_http_response(&response, 404, "application/json", "{\"error\": \"User not found\"}");
    }
    send_http_response(client_socket, &response);
}
void handle_post_user(const char* body, SOCKET client_socket) {
    HttpResponse response;
    json parsed_json;
    try {
        parsed_json = json::parse(body);
    } catch (json::parse_error& e) {
        prepare_http_response(&response, 400, "application/json", "{\"error\": \"Invalid JSON format\"}");
        send_http_response(client_socket, &response);
        return;
    }
    if (!parsed_json.contains("firstName") || !parsed_json.contains("lastName") ||
        !parsed_json.contains("birthYear") || !parsed_json.contains("group")) {
        prepare_http_response(&response, 400, "application/json", "{\"error\": \"Missing required fields\"}");
        send_http_response(client_socket, &response);
        return;
    }
    if (!parsed_json["firstName"].is_string() || !parsed_json["lastName"].is_string() ||
        !parsed_json["birthYear"].is_number_integer() || !parsed_json["group"].is_string()) {
        prepare_http_response(&response, 400, "application/json", "{\"error\": \"Invalid field types\"}");
        send_http_response(client_socket, &response);
        return;
    }
    const std::string& firstName = parsed_json["firstName"];
    const std::string& lastName = parsed_json["lastName"];
    int birthYear = parsed_json["birthYear"];
    const std::string& group = parsed_json["group"];
    if (!is_valid_group(group.c_str())) {
        prepare_http_response(&response, 400, "application/json", "{\"error\": \"Invalid group value. Must be 'user', 'premium', or 'admin'\"}");
        send_http_response(client_socket, &response);
        return;
    }
    int new_id = add_user(firstName.c_str(), lastName.c_str(), birthYear, group.c_str());
    if (new_id > 0) {
        User* newUser = get_user_by_id(new_id);
        json response_body = {
            {"id", newUser->id},
            {"firstName", newUser->firstName},
            {"lastName", newUser->lastName},
            {"age", calculate_age(newUser->birthYear)},
            {"group", newUser->group}
        };
        std::string response_str = response_body.dump();
        prepare_http_response(&response, 201, "application/json", response_str.c_str());
    } else {
        prepare_http_response(&response, 400, "application/json", "{\"error\": \"Failed to create user\"}");
    }
    send_http_response(client_socket, &response);
}
void handle_patch_user(int user_id, const char* body, SOCKET client_socket) {
    HttpResponse response;
    json parsed_json;
    try {
        parsed_json = json::parse(body);
    } catch (json::parse_error& e) {
        prepare_http_response(&response, 400, "application/json", "{\"error\": \"Invalid JSON format\"}");
        send_http_response(client_socket, &response);
        return;
    }

    std::string firstName_str, lastName_str, group_str;
    const char* firstName = nullptr;
    const char* lastName = nullptr;
    const char* group = nullptr;
    int birthYear = 0;

    if (parsed_json.contains("firstName")) {
        firstName_str = parsed_json["firstName"].get<std::string>();
        firstName = firstName_str.c_str();
    }
    if (parsed_json.contains("lastName")) {
        lastName_str = parsed_json["lastName"].get<std::string>();
        lastName = lastName_str.c_str();
    }
    if (parsed_json.contains("birthYear")) {
        birthYear = parsed_json["birthYear"].get<int>();
    }
    if (parsed_json.contains("group")) {
        group_str = parsed_json["group"].get<std::string>();
        group = group_str.c_str();
    }

    // Zmiana: zamiast 400 zwracamy 404, gdy wartość grupy jest nieprawidłowa
    if (group && !is_valid_group(group)) {
        prepare_http_response(&response, 404, "application/json", "{\"error\": \"Invalid group value. Must be 'user', 'premium', or 'admin'\"}");
        send_http_response(client_socket, &response);
        return;
    }

    if (update_user_partial(user_id, firstName, lastName, birthYear, group)) {
        User* updatedUser = get_user_by_id(user_id);
        json response_body = {
            {"id", updatedUser->id},
            {"firstName", updatedUser->firstName},
            {"lastName", updatedUser->lastName},
            {"age", calculate_age(updatedUser->birthYear)},
            {"group", updatedUser->group}
        };
        std::string response_str = response_body.dump();
        prepare_http_response(&response, 200, "application/json", response_str.c_str());
    } else {
        prepare_http_response(&response, 404, "application/json", "{\"error\": \"User not found or invalid data provided\"}");
    }

    send_http_response(client_socket, &response);
}


void handle_delete_user(int user_id, SOCKET client_socket) {
    HttpResponse response;
    if (delete_user(user_id)) {
        prepare_http_response(&response, 204, "application/json", "");
    } else {
        prepare_http_response(&response, 404, "application/json", "{\"error\": \"User not found\"}");
    }
    send_http_response(client_socket, &response);
}
