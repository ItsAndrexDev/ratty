cmake_minimum_required(VERSION 3.5)
project(ratty)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_PDB_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/symbols")


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

file(GLOB_RECURSE ADMIN_SOURCES CONFIGURE_DEPENDS
        admin/*.cpp
        deps/*.cpp
)

add_executable(server ${SERVER_SOURCES})
add_executable(client ${CLIENT_SOURCES})
add_executable(admin ${ADMIN_SOURCES})

add_custom_command(
        TARGET server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/libs"
        "$<TARGET_FILE_DIR:server>"
)