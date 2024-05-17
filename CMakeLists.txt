cmake_minimum_required(VERSION 3.26)
project(c_big_int)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_library(bigint big_int.c)
target_link_libraries(bigint m)
target_include_directories(bigint PUBLIC include)

add_executable(main main.c)
target_link_libraries(main bigint)

add_executable(test test.c)
target_link_libraries(test bigint)

