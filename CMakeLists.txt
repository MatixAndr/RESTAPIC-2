cmake_minimum_required(VERSION 3.30)
project(RESTAPIC_2)

set(CMAKE_CXX_STANDARD 17)

add_executable(RESTAPIC_2
    main.cpp
    routes.cpp
    server.cpp
    logger.cpp
    test_framework.cpp
    integration_test.cpp
    unit_test.cpp
)
