//
// Created by MateuszAndruszkiewic on 3.12.2024.
//

#include "test_framework.h"
#include "../src/user.h"
#include <iostream>

void test_add_user() {
    int id1 = add_user("Jan", "Kowalski");
    ASSERT_EQ(id1, 1);
    int id2 = add_user("Anna", "Nowak");
    ASSERT_EQ(id2, 2);
    int id3 = add_user("Piotr", "Zieliński");
    ASSERT_EQ(id3, 3);
    int id4 = add_user("Marek", "Kowalczyk");
    ASSERT_EQ(id4, 4);
    int id5 = add_user("Ewa", "Wiśniewska");
    ASSERT_EQ(id5, 5);
}

void test_get_user_by_id() {
    User* user1 = get_user_by_id(1);
    ASSERT_TRUE(user1 != nullptr);
    ASSERT_EQ(std::string(user1->name), "Jan");
    ASSERT_EQ(std::string(user1->lastname), "Kowalski");

    User* user2 = get_user_by_id(2);
    ASSERT_TRUE(user2 != nullptr);
    ASSERT_EQ(std::string(user2->name), "Anna");
    ASSERT_EQ(std::string(user2->lastname), "Nowak");

    User* user3 = get_user_by_id(3);
    ASSERT_TRUE(user3 != nullptr);
    ASSERT_EQ(std::string(user3->name), "Piotr");
    ASSERT_EQ(std::string(user3->lastname), "Zieliński");

    User* user4 = get_user_by_id(4);
    ASSERT_TRUE(user4 != nullptr);
    ASSERT_EQ(std::string(user4->name), "Marek");
    ASSERT_EQ(std::string(user4->lastname), "Kowalczyk");

    User* user5 = get_user_by_id(5);
    ASSERT_TRUE(user5 != nullptr);
    ASSERT_EQ(std::string(user5->name), "Ewa");
    ASSERT_EQ(std::string(user5->lastname), "Wiśniewska");
}

void test_update_user_partial() {
    int result1 = update_user_partial(1, "Jan2", nullptr);
    ASSERT_EQ(result1, 1);
    User* user1 = get_user_by_id(1);
    ASSERT_EQ(std::string(user1->name), "Jan2");
    ASSERT_EQ(std::string(user1->lastname), "Kowalski");

    int result2 = update_user_partial(2, nullptr, "Nowak2");
    ASSERT_EQ(result2, 1);
    User* user2 = get_user_by_id(2);
    ASSERT_EQ(std::string(user2->name), "Anna");
    ASSERT_EQ(std::string(user2->lastname), "Nowak2");

    int result3 = update_user_partial(3, "Piotr2", "Zieliński2");
    ASSERT_EQ(result3, 1);
    User* user3 = get_user_by_id(3);
    ASSERT_EQ(std::string(user3->name), "Piotr2");
    ASSERT_EQ(std::string(user3->lastname), "Zieliński2");

    int result4 = update_user_partial(4, "Marek2", nullptr);
    ASSERT_EQ(result4, 1);
    User* user4 = get_user_by_id(4);
    ASSERT_EQ(std::string(user4->name), "Marek2");
    ASSERT_EQ(std::string(user4->lastname), "Kowalczyk");

    int result5 = update_user_partial(5, nullptr, "Wiśniewska2");
    ASSERT_EQ(result5, 1);
    User* user5 = get_user_by_id(5);
    ASSERT_EQ(std::string(user5->name), "Ewa");
    ASSERT_EQ(std::string(user5->lastname), "Wiśniewska2");
}

void test_delete_user() {
    int result1 = delete_user(1);
    ASSERT_EQ(result1, 1);
    User* user1 = get_user_by_id(1);
    ASSERT_TRUE(user1 == nullptr);

    int result2 = delete_user(2);
    ASSERT_EQ(result2, 1);
    User* user2 = get_user_by_id(2);
    ASSERT_TRUE(user2 == nullptr);

    int result3 = delete_user(3);
    ASSERT_EQ(result3, 1);
    User* user3 = get_user_by_id(3);
    ASSERT_TRUE(user3 == nullptr);

    int result4 = delete_user(4);
    ASSERT_EQ(result4, 1);
    User* user4 = get_user_by_id(4);
    ASSERT_TRUE(user4 == nullptr);

    int result5 = delete_user(5);
    ASSERT_EQ(result5, 1);
    User* user5 = get_user_by_id(5);
    ASSERT_TRUE(user5 == nullptr);
}

int run_unit_tests() {
    TestSuite suite;
    suite.add_test("test_add_user", test_add_user);
    suite.add_test("test_get_user_by_id", test_get_user_by_id);
    suite.add_test("test_update_user_partial", test_update_user_partial);
    suite.add_test("test_delete_user", test_delete_user);
    suite.run();
    return 0;
}
