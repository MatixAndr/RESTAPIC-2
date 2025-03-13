cmake_minimum_required(VERSION 3.10)
project(RESTAPIC_2 VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(src)
include_directories(tests)

file(GLOB SOURCE_FILES "src/*.cpp" "src/*.h")

option(ENABLE_TESTS "Include test code in main executable" ON)
if(ENABLE_TESTS)
    file(GLOB TEST_FILES "tests/*.cpp" "tests/*.h")
    list(APPEND SOURCE_FILES ${TEST_FILES})
endif()

add_executable(RESTAPIC_2 ${SOURCE_FILES})
add_executable(RESTAPIC_2_tests ${SOURCE_FILES})

if(ENABLE_TESTS)
    target_compile_definitions(RESTAPIC_2 PRIVATE ENABLE_TESTS)
endif()

target_link_libraries(RESTAPIC_2 ws2_32)
target_link_libraries(RESTAPIC_2_tests ws2_32)

