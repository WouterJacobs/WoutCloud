cmake_minimum_required(VERSION 3.26)
project(WoutCloud C)

set(CMAKE_C_STANDARD 11)

add_executable(WoutCloud src/main.c)

target_link_libraries(WoutCloud ws2_32)
