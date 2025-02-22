cmake_minimum_required(VERSION 3.30)
project(RESTAPIC_2)

set(CMAKE_CXX_STANDARD 17)

add_executable(RESTAPIC_2
    main.cpp
    src/server/routes.cpp
    src/server/server.cpp
    src/util/logger.cpp
    src/util/test_framework.cpp
    tests/integration_test.cpp
    tests/unit_test.cpp
)
