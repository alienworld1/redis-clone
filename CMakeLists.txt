cmake_minimum_required(VERSION 3.31)
project(redis_clone)

set(CMAKE_CXX_STANDARD 23)

add_executable(redis_clone main.cpp)

if (WIN32)
    target_link_libraries(redis_clone PRIVATE ws2_32)
endif()
