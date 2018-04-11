cmake_minimum_required(VERSION 3.10)
project(Assignment_1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pthread")

add_executable(Assignment_1 main.cpp ThreadPool.h ThreadPool.h ServerSocket.h ClientSocket.h Query.h)