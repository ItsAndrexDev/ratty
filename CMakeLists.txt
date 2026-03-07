cmake_minimum_required(VERSION 4.3)
project(ratty)
add_compile_options(-Wno-microsoft-include)
add_compile_definitions(_WIN32_WINNT=0x0601)
file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
    src_server/*.cpp
)
file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
    src_client/*.cpp
)
add_executable(server ${SERVER_SOURCES})
add_executable(client ${CLIENT_SOURCES})