#include <string.h>
#include <stdio.h>
#include <time.h>
#include "user.h"

static User users[MAX_USERS];
static int user_count = 0;

int is_valid_group(const char* group) {
    return (strcmp(group, "user") == 0 ||
            strcmp(group, "premium") == 0 ||
            strcmp(group, "admin") == 0);
}
int calculate_age(int birthYear) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    int currentYear = tm.tm_year + 1900;
    return currentYear - birthYear;
}

int add_user(const char* firstName, const char* lastName, int birthYear, const char* group) {
    if (user_count >= MAX_USERS || !is_valid_group(group)) {
        return -1;
    }
    User new_user;
    new_user.id = user_count + 1;
    new_user.birthYear = birthYear;
    strncpy(new_user.firstName, firstName, MAX_NAME_LENGTH - 1);
    strncpy(new_user.lastName, lastName, MAX_NAME_LENGTH - 1);
    strncpy(new_user.group, group, MAX_GROUP_LENGTH - 1);
    new_user.firstName[MAX_NAME_LENGTH - 1] = '\0';
    new_user.lastName[MAX_NAME_LENGTH - 1] = '\0';
    new_user.group[MAX_GROUP_LENGTH - 1] = '\0';
    users[user_count++] = new_user;
    return new_user.id;
}
User* get_user_by_id(int id) {
    for (int i = 0; i < user_count; i++) {
        if (users[i].id == id) {
            return &users[i];
        }
    }
    return NULL;
}
int update_user_partial(int id, const char* firstName, const char* lastName, int birthYear, const char* group) {
    User* user = get_user_by_id(id);
    if (!user) return 0;
    if (firstName) strncpy(user->firstName, firstName, MAX_NAME_LENGTH - 1);
    if (lastName) strncpy(user->lastName, lastName, MAX_NAME_LENGTH - 1);
    if (birthYear > 0) user->birthYear = birthYear;
    if (group) {
        if (is_valid_group(group)) {
            strncpy(user->group, group, MAX_GROUP_LENGTH - 1);
        } else {
            return 0;
        }
    }
    return 1;
}
int delete_user(int id) {
    for (int i = 0; i < user_count; i++) {
        if (users[i].id == id) {
            for (int j = i; j < user_count - 1; j++) {
                users[j] = users[j + 1];
            }
            user_count--;
            return 1;
        }
    }
    return 0;
}
User* get_all_users(int* count) {
    *count = user_count;
    return users;
}
