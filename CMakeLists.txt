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

include_directories(src)

set(THREADS_PREFER_PTHREAD_FLAG ON)

add_library(SFT STATIC
        src/constants_storage.h

        src/app/Application.h
        src/app/Application.cpp

        src/network/Server.h
        src/network/Server.cpp
        src/network/Client.h
        src/network/Client.cpp

        src/crypto/Crypto.h
        src/crypto/Crypto.cpp

        src/util/parser/command_line_parser.h
        src/util/parser/command_line_parser.cpp

        src/util/logger/logger.h
        src/config.h src/util/logger/logger.cpp)

add_executable(secure_file_transfer src/main.cpp)

target_link_libraries(secure_file_transfer PRIVATE
        SFT
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto
        CONAN_PKG::boost)


enable_testing()

# Smoke Test
add_executable(smoke_test tests/Application/smoke_test.cpp)

target_link_libraries(smoke_test PRIVATE
        SFT
        CONAN_PKG::gtest
        CONAN_PKG::boost
        Threads::Threads
        OpenSSL::SSL
        OpenSSL::Crypto)

add_test(NAME SMOKE_TEST COMMAND smoke_test)


# Parser Test
add_executable(ParserTest src/util/parser/command_line_parser.cpp
        tests/CommandLineParser/test_framework_for_parser.cpp
        tests/CommandLineParser/client_config_tests.cpp
        tests/CommandLineParser/server_config_tests.cpp
        tests/CommandLineParser/general_tests.cpp)

target_link_libraries(ParserTest PRIVATE CONAN_PKG::catch2 CONAN_PKG::boost)

add_test(NAME PARSER_TEST COMMAND ParserTest)
