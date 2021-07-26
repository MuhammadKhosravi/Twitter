cmake_minimum_required(VERSION 3.17)
project(Twitter_Server C)

set(CMAKE_C_STANDARD 11)

add_executable(Twitter_Server main.c cJSON.c)