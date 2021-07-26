cmake_minimum_required(VERSION 3.17)
project(Twitter C)

set(CMAKE_C_STANDARD 11)

add_executable(Twitter main.c cJSON.c)
