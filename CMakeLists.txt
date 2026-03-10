cmake_minimum_required(VERSION 3.5)
project(ratty)

set(CMAKE_CXX_STANDARD 23)

add_compile_definitions(_WIN32_WINNT=0x0601)

include_directories(${CMAKE_SOURCE_DIR}/deps/asio)

file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
        server/*.cpp
        deps/*.cpp
)

file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
        client/*.cpp
        deps/*.cpp
)

add_executable(server ${SERVER_SOURCES})
add_executable(client ${CLIENT_SOURCES})