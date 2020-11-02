cmake_minimum_required(VERSION 3.15)
project(xlsx2struct)

set(CMAKE_CXX_STANDARD 14)

# log
include_directories(modules/LOG)

# xlnt
option(STATIC "static link" ON)
option(TESTS "xlnt tests" OFF)
add_subdirectory(modules/xlnt)
link_libraries(xlnt)

add_executable(xlsx2struct main.cpp)
