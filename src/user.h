#ifndef USER_H
#define USER_H

#define MAX_USERS 100
#define MAX_NAME_LENGTH 64
#define MAX_GROUP_LENGTH 16

typedef struct {
    int id;
    char firstName[MAX_NAME_LENGTH];
    char lastName[MAX_NAME_LENGTH];
    int birthYear;
    char group[MAX_GROUP_LENGTH];
} User;

int is_valid_group(const char* group);
int calculate_age(int birthYear);
int add_user(const char* firstName, const char* lastName, int birthYear, const char* group);

User* get_user_by_id(int id);

int update_user_partial(int id, const char* firstName, const char* lastName, int birthYear, const char* group);
int delete_user(int id);

User* get_all_users(int* count);

#endif // USER_H