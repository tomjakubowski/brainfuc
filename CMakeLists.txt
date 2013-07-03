cmake_minimum_required(VERSION 2.8)
project("brainfuc")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -Wall -std=c99 -pedantic")
add_subdirectory("${PROJECT_SOURCE_DIR}/src")
