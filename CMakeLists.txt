cmake_minimum_required(VERSION 3.11)

project(SFT CXX)
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


add_executable(safe_file_transfer
        src/main.cpp

        src/constants_storage.h
        src/app/Application.h
        src/network/Server.h
        src/network/Client.h
        src/crypto/Crypto.h
        src/util/parser/command_line_parser.h
        src/util/logger/logger.h

        src/app/Application.cpp
        src/network/Client.cpp
        src/network/Server.cpp
        src/crypto/Crypto.cpp
        src/util/parser/command_line_parser.cpp
        src/config.h src/util/logger/logger.cpp)

target_link_libraries(safe_file_transfer PRIVATE
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
        CONAN_PKG::boost)

# Tests
enable_testing()
add_executable(ParserTest src/util/parser/command_line_parser.cpp
        tests/CommandLineParser/test_framework_for_parser.cpp
        tests/CommandLineParser/client_config_tests.cpp
        tests/CommandLineParser/server_config_tests.cpp
        tests/CommandLineParser/general_tests.cpp)

target_link_libraries(ParserTest PRIVATE CONAN_PKG::catch2 CONAN_PKG::boost)
add_test(NAME PARSER_TEST COMMAND ParserTest)
