cmake_minimum_required(VERSION 3.11)

project(safe_tunnel CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Boost 1.78.0 REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif ()


set(THREADS_PREFER_PTHREAD_FLAG ON)


add_executable(safe_tunnel
        src/main.cpp

        src/constants_storage.h
        src/Application/Application.h
        src/Network/Server.h
        src/Network/Client.h
        src/Crypto/Crypto.h
        src/util/command_line_parser.h

        src/Application/Application.cpp
        src/Network/Client.cpp
        src/Network/Server.cpp
        src/Crypto/Crypto.cpp
        src/util/command_line_parser.cpp)

target_link_libraries(safe_tunnel PRIVATE
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
        CONAN_PKG::boost)

# Tests
enable_testing()
add_executable(ParserTest
        tests/CommandLineParser/client_config_tests.cpp src/util/command_line_parser.h src/util/command_line_parser.cpp tests/CommandLineParser/server_config_tests.cpp tests/CommandLineParser/test_framework_for_parser.cpp tests/CommandLineParser/test_framework_for_parser.h tests/CommandLineParser/general_tests.cpp)
target_link_libraries(ParserTest PRIVATE CONAN_PKG::catch2  CONAN_PKG::boost )
add_test(NAME PARSER_TEST COMMAND ParserTest)
