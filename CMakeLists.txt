cmake_minimum_required(VERSION 3.10)
project(RESTAPIC_2_2 VERSION 1.0)

# Ustawienie standardu C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Katalogi źródłowe
include_directories(src)
include_directories(tests)

# Pliki źródłowe
file(GLOB SOURCE_FILES
        "src/*.cpp"
        "src/*.h"
)

# Plik wykonywalny główny
add_executable(RESTAPIC_2 ${SOURCE_FILES})

# Biblioteki Winsock (wymagane dla Windows)
target_link_libraries(RESTAPIC_2 ws2_32)

# Pliki dla testów
file(GLOB TEST_FILES
        "tests/*.cpp"
        "tests/*.h"
)

# Plik wykonywalny testów
add_executable(RESTAPIC_2_tests ${SOURCE_FILES} ${TEST_FILES})

# Biblioteki dla testów
target_link_libraries(RESTAPIC_2_tests ws2_32)

# Dodanie stałych dla daty i autora
add_compile_definitions(
        RESTAPIC_2_BUILD_DATE="2025-03-06 22:07:31"
        RESTAPIC_2_AUTHOR="MatixAndr"
)