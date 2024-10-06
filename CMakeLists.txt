cmake_minimum_required(VERSION 3.29)
project(GnomeCS C)

set(CMAKE_C_STANDARD 11)

include_directories(./include)

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/source/*.c")

add_library(GnomeCSLib SHARED ${PROJECT_SOURCES})
target_include_directories(GnomeCSLib PRIVATE ./include)

add_executable(GnomeCSTest test/test.c ${PROJECT_SOURCES})
target_include_directories(GnomeCSTest PRIVATE ./include)