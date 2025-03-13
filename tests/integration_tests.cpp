#include "test_framework.h"
#include "../src/json.hpp"
#include "../src/routes.h"
#include "../src/server.h"
#include "../src/user.h"
#include <winsock2.h>
#include <string.h>
#include <stdio.h>

using json = nlohmann::json;

typedef struct {
    char response_buffer[MAX_RESPONSE_LENGTH];
    int response_length;
} MockSocket;

int mock_send(MockSocket* mock_socket, const char* data, int data_length, int flags) {
    mock_socket->response_length = data_length < MAX_RESPONSE_LENGTH ? data_length : MAX_RESPONSE_LENGTH - 1;
    memcpy(mock_socket->response_buffer, data, mock_socket->response_length);
    mock_socket->response_buffer[mock_socket->response_length] = '\0';
    return data_length;
}
const char* extract_body(const char* response) {
    const char* body_start = strstr(response, "\r\n\r\n");
    return body_start ? body_start + 4 : NULL;
}
int extract_status_code(const char* response) {
    int status_code;
    sscanf(response, "HTTP/1.1 %d", &status_code);
    return status_code;
}
void reset_users_for_integration() {
    int count;
    User* users = get_all_users(&count);
    for (int i = count - 1; i >= 0; i--) {
        delete_user(users[i].id);
    }
}
void run_integration_tests() {
    TEST_SUITE("API Endpoints");
    TEST_SETUP {
        reset_users_for_integration();
    });
    TEST_CASE("GET /users (empty)") {
        const char* request = "GET /users HTTP/1.1\r\nHost: localhost:8888\r\n\r\n";
        MockSocket mock_socket = {0};
        handle_request(request, (SOCKET)&mock_socket);
        EXPECT(extract_status_code(mock_socket.response_buffer) == 200);
        const char* body = extract_body(mock_socket.response_buffer);
        EXPECT(body != NULL);
        json response_json = json::parse(body);
        EXPECT(response_json.is_array());
        EXPECT(response_json.empty());
    });
    TEST_CASE("POST /users with valid data") {
        const char* post_request =
            "POST /users HTTP/1.1\r\n"
            "Host: localhost:8888\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{\"firstName\":\"John\",\"lastName\":\"Doe\",\"birthYear\":2000,\"group\":\"user\"}";
        MockSocket post_socket = {0};
        handle_request(post_request, (SOCKET)&post_socket);
        EXPECT(extract_status_code(post_socket.response_buffer) == 201);
        const char* body = extract_body(post_socket.response_buffer);
        json response_json = json::parse(body);
        EXPECT(response_json["firstName"] == "John");
        EXPECT(response_json["lastName"] == "Doe");
    });
    TEST_CASE("GET /users with multiple users") {
        add_user("User1", "Last1", 1990, "user");
        add_user("User2", "Last2", 1991, "premium");
        add_user("User3", "Last3", 1992, "admin");
        const char* request = "GET /users HTTP/1.1\r\nHost: localhost:8888\r\n\r\n";
        MockSocket mock_socket = {0};
        handle_request(request, (SOCKET)&mock_socket);
        EXPECT(extract_status_code(mock_socket.response_buffer) == 200);
        const char* body = extract_body(mock_socket.response_buffer);
        json response_json = json::parse(body);
        EXPECT(response_json.is_array());
        EXPECT(response_json.size() == 3);
    });
    TEST_CASE("GET /users/:id with valid ID") {
        add_user("John", "Doe", 2000, "user");
        add_user("Jane", "Smith", 1995, "premium");
        const char* request = "GET /users/2 HTTP/1.1\r\nHost: localhost:8888\r\n\r\n";
        MockSocket mock_socket = {0};
        handle_request(request, (SOCKET)&mock_socket);
        EXPECT(extract_status_code(mock_socket.response_buffer) == 200);
        const char* body = extract_body(mock_socket.response_buffer);
        json response_json = json::parse(body);
        EXPECT(response_json["id"] == 2);
        EXPECT(response_json["firstName"] == "Jane");
    });
    TEST_CASE("GET /users/:id with invalid ID") {
        const char* request = "GET /users/999 HTTP/1.1\r\nHost: localhost:8888\r\n\r\n";
        MockSocket mock_socket = {0};
        handle_request(request, (SOCKET)&mock_socket);
        EXPECT(extract_status_code(mock_socket.response_buffer) == 404);
    });
    TEST_CASE("PATCH /users/:id with valid data") {
        add_user("Original", "Name", 2000, "user");
        const char* patch_request =
            "PATCH /users/1 HTTP/1.1\r\n"
            "Host: localhost:8888\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{\"firstName\":\"Updated\",\"group\":\"premium\"}";
        MockSocket patch_socket = {0};
        handle_request(patch_request, (SOCKET)&patch_socket);
        EXPECT(extract_status_code(patch_socket.response_buffer) == 200);
    });
    TEST_CASE("PATCH /users/:id with invalid ID") {
        const char* patch_request =
            "PATCH /users/999 HTTP/1.1\r\n"
            "Host: localhost:8888\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{\"firstName\":\"Updated\"}";
        MockSocket patch_socket = {0};
        handle_request(patch_request, (SOCKET)&patch_socket);
        EXPECT(extract_status_code(patch_socket.response_buffer) == 404);
    });
    TEST_CASE("PATCH /users/:id with invalid group") {
        add_user("Original", "Name", 2000, "user");
        const char* patch_request =
            "PATCH /users/1 HTTP/1.1\r\n"
            "Host: localhost:8888\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{\"group\":\"invalid_group\"}";
        MockSocket patch_socket = {0};
        handle_request(patch_request, (SOCKET)&patch_socket);
        EXPECT(extract_status_code(patch_socket.response_buffer) == 404);
    });
    TEST_CASE("DELETE /users/:id with valid ID") {
        add_user("John", "Doe", 2000, "user");
        const char* delete_request = "DELETE /users/1 HTTP/1.1\r\nHost: localhost:8888\r\n\r\n";
        MockSocket delete_socket = {0};
        handle_request(delete_request, (SOCKET)&delete_socket);
        EXPECT(extract_status_code(delete_socket.response_buffer) == 204);
    });
    TEST_CASE("DELETE /users/:id with invalid ID") {
        const char* delete_request = "DELETE /users/999 HTTP/1.1\r\nHost: localhost:8888\r\n\r\n";
        MockSocket delete_socket = {0};
        handle_request(delete_request, (SOCKET)&delete_socket);
        EXPECT(extract_status_code(delete_socket.response_buffer) == 404);
    });
    TEST_CASE("POST /users with invalid JSON") {
        const char* request =
            "POST /users HTTP/1.1\r\n"
            "Host: localhost:8888\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{invalid_json}";
        MockSocket mock_socket = {0};
        handle_request(request, (SOCKET)&mock_socket);
        EXPECT(extract_status_code(mock_socket.response_buffer) == 400);
    });
    TEST_CASE("POST /users with missing required fields") {
        const char* request =
            "POST /users HTTP/1.1\r\n"
            "Host: localhost:8888\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{\"firstName\":\"John\"}";
        MockSocket mock_socket = {0};
        handle_request(request, (SOCKET)&mock_socket);
        EXPECT(extract_status_code(mock_socket.response_buffer) == 400);
    });
    TEST_CASE("POST /users with invalid group") {
        const char* request =
            "POST /users HTTP/1.1\r\n"
            "Host: localhost:8888\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{\"firstName\":\"John\",\"lastName\":\"Doe\",\"birthYear\":2000,\"group\":\"invalid\"}";
        MockSocket mock_socket = {0};
        handle_request(request, (SOCKET)&mock_socket);
        EXPECT(extract_status_code(mock_socket.response_buffer) == 400);
    });
    TEST_CASE("Invalid endpoint") {
        const char* request = "GET /invalid HTTP/1.1\r\nHost: localhost:8888\r\n\r\n";
        MockSocket mock_socket = {0};
        handle_request(request, (SOCKET)&mock_socket);
        EXPECT(extract_status_code(mock_socket.response_buffer) == 404);
    });
    TEST_CASE("Invalid method") {
        const char* request = "PUT /users HTTP/1.1\r\nHost: localhost:8888\r\n\r\n";
        MockSocket mock_socket = {0};
        handle_request(request, (SOCKET)&mock_socket);
        EXPECT(extract_status_code(mock_socket.response_buffer) == 404);
    });
    TestFramework::run_all();
}
