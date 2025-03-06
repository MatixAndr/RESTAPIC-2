//
// Created by MateuszAndruszkiewic on 3.12.2024.
//

#include "test_framework.h"
#include "../src/user.h"
#include "../src/server.h"
#include <iostream>
#include "../src/routes.h"

void test_http_request_parsing_1() {
    const char* raw_request = "GET /users HTTP/1.1\r\nHost: localhost\r\n\r\n";
    HttpRequest request;
    int result = parse_http_request(raw_request, &request);
    ASSERT_EQ(result, 1);
    ASSERT_EQ(std::string(request.method), "GET");
    ASSERT_EQ(std::string(request.path), "/users");
    ASSERT_EQ(std::string(request.protocol), "HTTP/1.1");
}

void test_http_request_parsing_2() {
    const char* raw_request = "POST /users HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/json\r\n\r\n{\"name\":\"John\",\"lastname\":\"Doe\"}";
    HttpRequest request;
    int result = parse_http_request(raw_request, &request);
    ASSERT_EQ(result, 1);
    ASSERT_EQ(std::string(request.method), "POST");
    ASSERT_EQ(std::string(request.path), "/users");
    ASSERT_EQ(std::string(request.protocol), "HTTP/1.1");
    ASSERT_EQ(std::string(request.body), "{\"name\":\"John\",\"lastname\":\"Doe\"}");
}

void test_http_request_parsing_3() {
    const char* raw_request = "PUT /users/1 HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/json\r\n\r\n{\"name\":\"Jane\"}";
    HttpRequest request;
    int result = parse_http_request(raw_request, &request);
    ASSERT_EQ(result, 1);
    ASSERT_EQ(std::string(request.method), "PUT");
    ASSERT_EQ(std::string(request.path), "/users/1");
    ASSERT_EQ(std::string(request.protocol), "HTTP/1.1");
    ASSERT_EQ(std::string(request.body), "{\"name\":\"Jane\"}");
}

void test_http_request_parsing_4() {
    const char* raw_request = "DELETE /users/1 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    HttpRequest request;
    int result = parse_http_request(raw_request, &request);
    ASSERT_EQ(result, 1);
    ASSERT_EQ(std::string(request.method), "DELETE");
    ASSERT_EQ(std::string(request.path), "/users/1");
    ASSERT_EQ(std::string(request.protocol), "HTTP/1.1");
}

void test_http_request_parsing_5() {
    const char* raw_request = "GET /users?page=1&limit=10 HTTP/1.1\r\nHost: localhost\r\n\r\n";
    HttpRequest request;
    int result = parse_http_request(raw_request, &request);
    ASSERT_EQ(result, 1);
    ASSERT_EQ(std::string(request.method), "GET");
    ASSERT_EQ(std::string(request.path), "/users?page=1&limit=10");
    ASSERT_EQ(std::string(request.protocol), "HTTP/1.1");
}

void test_http_response_generation_1() {
    HttpResponse response;
    const char* test_body = "{\"test\": \"data\"}";
    prepare_http_response(&response, 200, "application/json", test_body);
    ASSERT_EQ(response.status_code, 200);
    ASSERT_EQ(std::string(response.status_message), "OK");
    ASSERT_EQ(std::string(response.content_type), "application/json");
    ASSERT_EQ(std::string(response.body), test_body);
}

void test_http_response_generation_2() {
    HttpResponse response;
    const char* test_body = "<html><body>Hello, World!</body></html>";
    prepare_http_response(&response, 200, "text/html", test_body);
    ASSERT_EQ(response.status_code, 200);
    ASSERT_EQ(std::string(response.status_message), "OK");
    ASSERT_EQ(std::string(response.content_type), "text/html");
    ASSERT_EQ(std::string(response.body), test_body);
}

void test_http_response_generation_3() {
    HttpResponse response;
    prepare_http_response(&response, 404, "text/plain", "Not Found");
    ASSERT_EQ(response.status_code, 404);
    ASSERT_EQ(std::string(response.status_message), "Not Found");
    ASSERT_EQ(std::string(response.content_type), "text/plain");
    ASSERT_EQ(std::string(response.body), "Not Found");
}

void test_http_response_generation_4() {
    HttpResponse response;
    prepare_http_response(&response, 500, "text/plain", "Internal Server Error");
    ASSERT_EQ(response.status_code, 500);
    ASSERT_EQ(std::string(response.status_message), "Internal Server Error");
    ASSERT_EQ(std::string(response.content_type), "text/plain");
    ASSERT_EQ(std::string(response.body), "Internal Server Error");
}

void test_http_response_generation_5() {
    HttpResponse response;
    prepare_http_response(&response, 201, "application/json", "{\"id\": 1, \"message\": \"Created\"}");
    ASSERT_EQ(response.status_code, 201);
    ASSERT_EQ(std::string(response.status_message), "Created");
    ASSERT_EQ(std::string(response.content_type), "application/json");
    ASSERT_EQ(std::string(response.body), "{\"id\": 1, \"message\": \"Created\"}");
}

void test_add_user_integration() {
    HttpRequest request;
    strcpy(request.method, "POST");
    strcpy(request.path, "/users");
    strcpy(request.body, "{\"name\":\"John\",\"lastname\":\"Doe\"}");

    char raw_request[4096];
    sprintf(raw_request, "%s %s HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/json\r\n\r\n%s",
            request.method, request.path, request.body);

    handle_request(raw_request, 0);

    HttpResponse response = get_last_response();
    ASSERT_EQ(response.status_code, 201);
    ASSERT_EQ(std::string(response.content_type), "application/json");
    ASSERT_TRUE(strstr(response.body, "\"id\":") != nullptr);
}

void test_get_user_integration() {
    int user_id = add_user("Jane", "Smith");

    HttpRequest request;
    strcpy(request.method, "GET");
    sprintf(request.path, "/users/%d", user_id);
    strcpy(request.body, "");

    char raw_request[4096];
    sprintf(raw_request, "%s %s HTTP/1.1\r\nHost: localhost\r\n\r\n%s", request.method, request.path, request.body);
    handle_request(raw_request, 0);

    HttpResponse response = get_last_response();
    ASSERT_EQ(response.status_code, 200);
    ASSERT_EQ(std::string(response.content_type), "application/json");
    ASSERT_TRUE(strstr(response.body, "\"name\":\"Jane\"") != nullptr);
    ASSERT_TRUE(strstr(response.body, "\"lastname\":\"Smith\"") != nullptr);
}

void test_update_user_integration() {
    int user_id = add_user("Bob", "Johnson");

    HttpRequest request;
    strcpy(request.method, "PUT");
    sprintf(request.path, "/users/%d", user_id);
    strcpy(request.body, "{\"name\":\"Robert\"}");

    char raw_request[4096];
    sprintf(raw_request, "%s %s HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/json\r\n\r\n%s", request.method, request.path, request.body);
    handle_request(raw_request, 0);

    HttpResponse response = get_last_response();
    ASSERT_EQ(response.status_code, 200);
    ASSERT_EQ(std::string(response.content_type), "application/json");
    ASSERT_TRUE(strstr(response.body, "\"name\":\"Robert\"") != nullptr);
    ASSERT_TRUE(strstr(response.body, "\"lastname\":\"Johnson\"") != nullptr);
}

void test_delete_user_integration() {
    int user_id = add_user("Alice", "Brown");

    HttpRequest request;
    strcpy(request.method, "DELETE");
    sprintf(request.path, "/users/%d", user_id);
    strcpy(request.body, "");

    char raw_request[4096];
    sprintf(raw_request, "%s %s HTTP/1.1\r\nHost: localhost\r\n\r\n%s", request.method, request.path, request.body);
    handle_request(raw_request, 0);

    HttpResponse response = get_last_response();
    ASSERT_EQ(response.status_code, 204);

    strcpy(request.method, "GET");
    sprintf(raw_request, "%s %s HTTP/1.1\r\nHost: localhost\r\n\r\n%s", request.method, request.path, request.body);
    handle_request(raw_request, 0);

    response = get_last_response();
    ASSERT_EQ(response.status_code, 404);
}

void test_get_all_users_integration() {
    add_user("User1", "Last1");
    add_user("User2", "Last2");
    add_user("User3", "Last3");

    HttpRequest request;
    strcpy(request.method, "GET");
    strcpy(request.path, "/users");
    strcpy(request.body, "");

    char raw_request[4096];
    sprintf(raw_request, "%s %s HTTP/1.1\r\nHost: localhost\r\n\r\n%s", request.method, request.path, request.body);
    handle_request(raw_request, 0);

    HttpResponse response = get_last_response();
    ASSERT_EQ(response.status_code, 200);
    ASSERT_EQ(std::string(response.content_type), "application/json");
    ASSERT_TRUE(strstr(response.body, "User1") != nullptr);
    ASSERT_TRUE(strstr(response.body, "User2") != nullptr);
    ASSERT_TRUE(strstr(response.body, "User3") != nullptr);
}

void test_add_user_invalid_json_integration() {
    HttpRequest request;
    strcpy(request.method, "POST");
    strcpy(request.path, "/users");
    strcpy(request.body, "{invalid_json}");

    char raw_request[4096];
    sprintf(raw_request, "%s %s HTTP/1.1\r\nHost: localhost\r\n\r\n%s", request.method, request.path, request.body);
    handle_request(raw_request, 0);

    HttpResponse response = get_last_response();
    ASSERT_EQ(response.status_code, 400);
    ASSERT_EQ(std::string(response.content_type), "application/json");
    ASSERT_TRUE(strstr(response.body, "error") != nullptr);
}

void test_get_nonexistent_user_integration() {
    HttpRequest request;
    strcpy(request.method, "GET");
    strcpy(request.path, "/users/9999");
    strcpy(request.body, "");

    char raw_request[4096];
    sprintf(raw_request, "%s %s HTTP/1.1\r\nHost: localhost\r\n\r\n%s", request.method, request.path, request.body);
    handle_request(raw_request, 0);

    HttpResponse response = get_last_response();
    ASSERT_EQ(response.status_code, 404);
    ASSERT_EQ(std::string(response.content_type), "application/json");
    ASSERT_TRUE(strstr(response.body, "error") != nullptr);
}

void test_update_nonexistent_user_integration() {
    HttpRequest request;
    strcpy(request.method, "PUT");
    strcpy(request.path, "/users/9999");
    strcpy(request.body, "{\"name\":\"NewName\"}");

    char raw_request[4096];
    sprintf(raw_request, "%s %s HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/json\r\n\r\n%s", request.method, request.path, request.body);
    handle_request(raw_request, 0);

    HttpResponse response = get_last_response();
    ASSERT_EQ(response.status_code, 404);
    ASSERT_EQ(std::string(response.content_type), "application/json");
    ASSERT_TRUE(strstr(response.body, "error") != nullptr);
}

void test_delete_nonexistent_user_integration() {
    HttpRequest request;
    strcpy(request.method, "DELETE");
    strcpy(request.path, "/users/9999");
    strcpy(request.body, "");

    char raw_request[4096];
    sprintf(raw_request, "%s %s HTTP/1.1\r\nHost: localhost\r\n\r\n%s", request.method, request.path, request.body);
    handle_request(raw_request, 0);

    HttpResponse response = get_last_response();
    ASSERT_EQ(response.status_code, 404);
    ASSERT_EQ(std::string(response.content_type), "application/json");
    ASSERT_TRUE(strstr(response.body, "error") != nullptr);
}

void test_invalid_http_method_integration() {
    HttpRequest request;
    strcpy(request.method, "INVALID");
    strcpy(request.path, "/users");
    strcpy(request.body, "");

    char raw_request[4096];
    sprintf(raw_request, "%s %s HTTP/1.1\r\nHost: localhost\r\n\r\n%s", request.method, request.path, request.body);
    handle_request(raw_request, 0);

    HttpResponse response = get_last_response();
    ASSERT_EQ(response.status_code, 405);
    ASSERT_EQ(std::string(response.content_type), "application/json");
    ASSERT_TRUE(strstr(response.body, "error") != nullptr);
}

void test_invalid_path_integration() {
    HttpRequest request;
    strcpy(request.method, "GET");
    strcpy(request.path, "/invalid_path");
    strcpy(request.body, "");

    char raw_request[4096];
    sprintf(raw_request, "%s %s HTTP/1.1\r\nHost: localhost\r\n\r\n%s", request.method, request.path, request.body);
    handle_request(raw_request, 0);

    HttpResponse response = get_last_response();
    ASSERT_EQ(response.status_code, 404);
    ASSERT_EQ(std::string(response.content_type), "application/json");
    ASSERT_TRUE(strstr(response.body, "error") != nullptr);
}

void test_server_error_simulation_integration() {
    HttpRequest request;
    strcpy(request.method, "POST");
    strcpy(request.path, "/users");
    strcpy(request.body, "{\"invalid_field\":\"value\"}");

    char raw_request[4096];
    sprintf(raw_request, "%s %s HTTP/1.1\r\nHost: localhost\r\n\r\n%s", request.method, request.path, request.body);
    handle_request(raw_request, 0);

    HttpResponse response = get_last_response();
    ASSERT_EQ(response.status_code, 500);
    ASSERT_EQ(std::string(response.content_type), "application/json");
    ASSERT_TRUE(strstr(response.body, "error") != nullptr);
}

void test_large_payload_integration() {
    HttpRequest request;
    strcpy(request.method, "POST");
    strcpy(request.path, "/users");
    char large_body[10001] = "{\"name\":\"";
    memset(large_body + 9, 'A', 9980);
    strcat(large_body, "\",\"lastname\":\"Doe\"}");
    strcpy(request.body, large_body);

    char raw_request[4096];
    sprintf(raw_request, "%s %s HTTP/1.1\r\nHost: localhost\r\n\r\n%s", request.method, request.path, request.body);
    handle_request(raw_request, 0);

    HttpResponse response = get_last_response();
    ASSERT_EQ(response.status_code, 413);
    ASSERT_EQ(std::string(response.content_type), "application/json");
    ASSERT_TRUE(strstr(response.body, "error") != nullptr);
}

void test_concurrent_requests_integration() {
    HttpRequest request1, request2;
    HttpResponse response1, response2;

    strcpy(request1.method, "POST");
    strcpy(request1.path, "/users");
    strcpy(request1.body, "{\"name\":\"User1\",\"lastname\":\"Last1\"}");

    strcpy(request2.method, "GET");
    strcpy(request2.path, "/users");
    strcpy(request2.body, "");

    char raw_request1[4096], raw_request2[4096];
    sprintf(raw_request1, "%s %s HTTP/1.1\r\nHost: localhost\r\nContent-Type: application/json\r\n\r\n%s", request1.method, request1.path, request1.body);
    sprintf(raw_request2, "%s %s HTTP/1.1\r\nHost: localhost\r\n\r\n%s", request2.method, request2.path, request2.body);

    handle_request(raw_request1, 0);
    response1 = get_last_response();
    handle_request(raw_request2, 0);
    response2 = get_last_response();

    ASSERT_EQ(response1.status_code, 201);
    ASSERT_EQ(response2.status_code, 200);
    ASSERT_TRUE(strstr(response2.body, "User1") != nullptr);
}

int run_integration_tests() {
    TestSuite suite;
    suite.add_test("test_http_request_parsing_1", test_http_request_parsing_1);
    suite.add_test("test_http_request_parsing_2", test_http_request_parsing_2);
    suite.add_test("test_http_request_parsing_3", test_http_request_parsing_3);
    suite.add_test("test_http_request_parsing_4", test_http_request_parsing_4);
    suite.add_test("test_http_request_parsing_5", test_http_request_parsing_5);
    suite.add_test("test_http_response_generation_1", test_http_response_generation_1);
    suite.add_test("test_http_response_generation_2", test_http_response_generation_2);
    suite.add_test("test_http_response_generation_3", test_http_response_generation_3);
    suite.add_test("test_http_response_generation_4", test_http_response_generation_4);
    suite.add_test("test_http_response_generation_5", test_http_response_generation_5);
    suite.add_test("test_add_user_integration", test_add_user_integration);
    suite.add_test("test_get_user_integration", test_get_user_integration);
    suite.add_test("test_get_all_users_integration", test_get_all_users_integration);
    suite.add_test("test_add_user_invalid_json_integration", test_add_user_invalid_json_integration);

    suite.run();
    return 0;
}