//
// Created by MateuszAndruszkiewic on 12.12.2024.
//

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <iostream>
#include <vector>
#include <functional>
#include <cstdlib>

struct Test {
    std::string name;
    std::function<void()> func;
};

class TestSuite {
public:
    void add_test(const std::string& name, std::function<void()> func) {
        tests.push_back({name, func});
    }

    int run() {
        int passed = 0;
        std::cout << "==================== Tests status: ====================" << std::endl;
        for (const auto& test : tests) {
            try {
                test.func();
                std::cout << "\033[32m( PASSED )\033[0m " << test.name << std::endl;
                passed++;
            } catch (const std::exception& e) {
                std::cout << "\033[31m( FAILED )\033[0m " << test.name << ": " << e.what() << std::endl;
            }
        }
        std::cout << "========== Passed " << passed << " out of " << tests.size() << " tests ==========" << std::endl;
        std::exit(passed == tests.size() ? 0 : 1);
    }

private:
    std::vector<Test> tests;
};

#define ASSERT_TRUE(cond) if (!(cond)) throw std::runtime_error("Assertion failed: " #cond)
#define ASSERT_EQ(val1, val2) if ((val1) != (val2)) throw std::runtime_error("Assertion failed: " #val1 " == " #val2)

#endif // TEST_FRAMEWORK_H