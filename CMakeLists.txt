cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 23)
project(ratty)


add_compile_definitions(_WIN32_WINNT=0x0601)



file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
    src_server/*.cpp
    deps/*.cpp
)
file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
    src_client/*.cpp
    deps/*.cpp
)


add_executable(server ${SERVER_SOURCES})
add_executable(client ${CLIENT_SOURCES})