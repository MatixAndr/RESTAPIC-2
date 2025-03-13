#include "test_framework.h"
#include "../src/server.h"
#include "../src/user.h"
#include <string.h>
#include <time.h>

void reset_user_data() {
    int count;
    User* users = get_all_users(&count);
    for (int i = count - 1; i >= 0; i--) {
        delete_user(users[i].id);
    }
}
void run_unit_tests() {
    TEST_SUITE("User Model");
    TEST_SETUP {
        reset_user_data();
    });
    TEST_CASE("Group validation - valid groups") {
        EXPECT(is_valid_group("user") == 1);
        EXPECT(is_valid_group("premium") == 1);
        EXPECT(is_valid_group("admin") == 1);
    });
    TEST_CASE("Group validation - invalid groups") {
        EXPECT(is_valid_group("invalid") == 0);
        EXPECT(is_valid_group("") == 0);
        EXPECT(is_valid_group("USER") == 0);
        EXPECT(is_valid_group("Administrator") == 0);
    });
    TEST_CASE("Age calculation") {
        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        int current_year = tm.tm_year + 1900;
        EXPECT(calculate_age(current_year - 20) == 20);
        EXPECT(calculate_age(current_year) == 0);
        EXPECT(calculate_age(current_year - 100) == 100);
    });
    TEST_CASE("User addition - success") {
        int id1 = add_user("John", "Doe", 2000, "user");
        EXPECT(id1 == 1);
        int id2 = add_user("Jane", "Smith", 1995, "premium");
        EXPECT(id2 == 2);
        int count;
        get_all_users(&count);
        EXPECT(count == 2);
    });
    TEST_CASE("User addition - all user types") {
        int id1 = add_user("User", "Normal", 2000, "user");
        EXPECT(id1 == 1);
        int id2 = add_user("Premium", "User", 1995, "premium");
        EXPECT(id2 == 2);
        int id3 = add_user("Admin", "User", 1985, "admin");
        EXPECT(id3 == 3);
        int count;
        get_all_users(&count);
        EXPECT(count == 3);
    });
    TEST_CASE("User addition - invalid group") {
        int id = add_user("Invalid", "Group", 2000, "invalid_group");
        EXPECT(id == -1);
        int count;
        get_all_users(&count);
        EXPECT(count == 0);
    });
    TEST_CASE("User retrieval - existing user") {
        int id = add_user("John", "Doe", 2000, "user");
        User* user = get_user_by_id(id);
        EXPECT(user != NULL);
        EXPECT(strcmp(user->firstName, "John") == 0);
        EXPECT(strcmp(user->lastName, "Doe") == 0);
        EXPECT(user->birthYear == 2000);
        EXPECT(strcmp(user->group, "user") == 0);
    });
    TEST_CASE("User retrieval - non-existing user") {
        User* nonexistent = get_user_by_id(999);
        EXPECT(nonexistent == NULL);
    });
    TEST_CASE("Get all users - empty") {
        int count;
        User* users = get_all_users(&count);
        EXPECT(count == 0);
    });
    TEST_CASE("Get all users - multiple users") {
        add_user("User1", "Last1", 1990, "user");
        add_user("User2", "Last2", 1991, "premium");
        add_user("User3", "Last3", 1992, "admin");
        int count;
        User* users = get_all_users(&count);
        EXPECT(count == 3);
    });
    TEST_CASE("User update - firstName only") {
        int id = add_user("Original", "Name", 2000, "user");
        int result = update_user_partial(id, "Updated", NULL, 0, NULL);
        EXPECT(result == 1);
        User* user = get_user_by_id(id);
        EXPECT(strcmp(user->firstName, "Updated") == 0);
        EXPECT(strcmp(user->lastName, "Name") == 0);
        EXPECT(user->birthYear == 2000);
        EXPECT(strcmp(user->group, "user") == 0);
    });
    TEST_CASE("User update - lastName only") {
        int id = add_user("Original", "Name", 2000, "user");
        int result = update_user_partial(id, NULL, "NewLastName", 0, NULL);
        EXPECT(result == 1);
        User* user = get_user_by_id(id);
        EXPECT(strcmp(user->firstName, "Original") == 0);
        EXPECT(strcmp(user->lastName, "NewLastName") == 0);
        EXPECT(user->birthYear == 2000);
        EXPECT(strcmp(user->group, "user") == 0);
    });
    TEST_CASE("User update - birthYear only") {
        int id = add_user("Original", "Name", 2000, "user");
        int result = update_user_partial(id, NULL, NULL, 1990, NULL);
        EXPECT(result == 1);
        User* user = get_user_by_id(id);
        EXPECT(strcmp(user->firstName, "Original") == 0);
        EXPECT(strcmp(user->lastName, "Name") == 0);
        EXPECT(user->birthYear == 1990);
        EXPECT(strcmp(user->group, "user") == 0);
    });
    TEST_CASE("User update - group only") {
        int id = add_user("Original", "Name", 2000, "user");
        int result = update_user_partial(id, NULL, NULL, 0, "premium");
        EXPECT(result == 1);
        User* user = get_user_by_id(id);
        EXPECT(strcmp(user->firstName, "Original") == 0);
        EXPECT(strcmp(user->lastName, "Name") == 0);
        EXPECT(user->birthYear == 2000);
        EXPECT(strcmp(user->group, "premium") == 0);
    });
    TEST_CASE("User update - multiple fields") {
        int id = add_user("Original", "Name", 2000, "user");
        int result = update_user_partial(id, "Updated", "NewLastName", 1990, "admin");
        EXPECT(result == 1);
        User* user = get_user_by_id(id);
        EXPECT(strcmp(user->firstName, "Updated") == 0);
        EXPECT(strcmp(user->lastName, "NewLastName") == 0);
        EXPECT(user->birthYear == 1990);
        EXPECT(strcmp(user->group, "admin") == 0);
    });
    TEST_CASE("User update - invalid group") {
        int id = add_user("Original", "Name", 2000, "user");
        int result = update_user_partial(id, NULL, NULL, 0, "invalid_group");
        EXPECT(result == 0);
        User* user = get_user_by_id(id);
        EXPECT(strcmp(user->group, "user") == 0);
    });
    TEST_CASE("User update - non-existing user") {
        int result = update_user_partial(999, "Test", NULL, 0, NULL);
        EXPECT(result == 0);
    });
    TEST_CASE("User deletion - existing user") {
        int id = add_user("John", "Doe", 2000, "user");
        int result = delete_user(id);
        EXPECT(result == 1);
        User* deleted_user = get_user_by_id(id);
        EXPECT(deleted_user == NULL);
    });
    TEST_CASE("User deletion - non-existing user") {
        int result = delete_user(999);
        EXPECT(result == 0);
    });
    TEST_CASE("User deletion - verify remaining users") {
        int id1 = add_user("User1", "Last1", 1990, "user");
        int id2 = add_user("User2", "Last2", 1991, "premium");
        int id3 = add_user("User3", "Last3", 1992, "admin");
        delete_user(id2);
        int count;
        User* users = get_all_users(&count);
        EXPECT(count == 2);
        User* user1 = get_user_by_id(id1);
        User* user2 = get_user_by_id(id2);
        User* user3 = get_user_by_id(id3);
        EXPECT(user1 != NULL);
        EXPECT(user2 == NULL);
        EXPECT(user3 != NULL);
    });
    TEST_SUITE("HTTP Parsing");
    TEST_CASE("HTTP request parsing - GET") {
        const char* valid_request =
            "GET /users HTTP/1.1\r\n"
            "Host: localhost:8888\r\n"
            "Content-Type: application/json\r\n"
            "\r\n";
        HttpRequest request;
        int result = parse_http_request(valid_request, &request);
        EXPECT(result == 1);
        EXPECT(strcmp(request.method, "GET") == 0);
        EXPECT(strcmp(request.path, "/users") == 0);
        EXPECT(strcmp(request.protocol, "HTTP/1.1") == 0);
    });
    TEST_CASE("HTTP request parsing - POST with body") {
        const char* request_with_body =
            "POST /users HTTP/1.1\r\n"
            "Host: localhost:8888\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"
            "{\"firstName\":\"John\",\"lastName\":\"Doe\",\"birthYear\":2000,\"group\":\"user\"}";
        HttpRequest request;
        int result = parse_http_request(request_with_body, &request);
        EXPECT(result == 1);
        EXPECT(strcmp(request.method, "POST") == 0);
        EXPECT(strcmp(request.path, "/users") == 0);
        EXPECT(strstr(request.body, "John") != NULL);
        EXPECT(strstr(request.body, "Doe") != NULL);
        EXPECT(strstr(request.body, "2000") != NULL);
        EXPECT(strstr(request.body, "user") != NULL);
    });
    TEST_CASE("HTTP response preparation") {
        HttpResponse response;
        prepare_http_response(&response, 200, "application/json", "{\"message\":\"Success\"}");
        EXPECT(response.status_code == 200);
        EXPECT(strcmp(response.content_type, "application/json") == 0);
        EXPECT(strcmp(response.body, "{\"message\":\"Success\"}") == 0);
        EXPECT(response.body_length == strlen("{\"message\":\"Success\"}"));
    });
    TEST_CASE("HTTP response preparation - empty body") {
        HttpResponse response;
        prepare_http_response(&response, 204, "application/json", "");
        EXPECT(response.status_code == 204);
        EXPECT(strcmp(response.content_type, "application/json") == 0);
        EXPECT(strcmp(response.body, "") == 0);
        EXPECT(response.body_length == 0);
    });
    TEST_CASE("HTTP response status text") {
        HttpResponse response;
        prepare_http_response(&response, 200, "text/plain", "OK");
        EXPECT(strcmp(get_status_text(response.status_code), "OK") == 0);
        prepare_http_response(&response, 201, "text/plain", "Created");
        EXPECT(strcmp(get_status_text(response.status_code), "Created") == 0);
        prepare_http_response(&response, 204, "text/plain", "No Content");
        EXPECT(strcmp(get_status_text(response.status_code), "No Content") == 0);
        prepare_http_response(&response, 400, "text/plain", "Bad Request");
        EXPECT(strcmp(get_status_text(response.status_code), "Bad Request") == 0);
        prepare_http_response(&response, 404, "text/plain", "Not Found");
        EXPECT(strcmp(get_status_text(response.status_code), "Not Found") == 0);
    });
    TEST_CASE("HTTP response formatting") {
        HttpResponse response;
        prepare_http_response(&response, 200, "application/json", "{\"test\":true}");
        char formatted_response[1024];
        format_response(&response, formatted_response);
        EXPECT(strstr(formatted_response, "HTTP/1.1 200 OK") != NULL);
        EXPECT(strstr(formatted_response, "Content-Type: application/json") != NULL);
        EXPECT(strstr(formatted_response, "Content-Length:") != NULL);
        EXPECT(strstr(formatted_response, "{\"test\":true}") != NULL);
    });
    TestFramework::run_all();
}
