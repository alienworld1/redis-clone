cmake_minimum_required(VERSION 3.31)
project(redis_clone)

set(CMAKE_CXX_STANDARD 23)

add_executable(client client.cpp
        utils/socketio.cpp
        utils/socketio.h)
add_executable(server server.cpp
        utils/socketio.cpp
        utils/socketio.h)

if (WIN32)
    target_link_libraries(server PRIVATE ws2_32)
    target_link_libraries(client PRIVATE ws2_32)
endif()

