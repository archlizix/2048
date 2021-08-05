cmake_minimum_required(VERSION 3.5)
project(2048)
add_executable(2048 main.cc)
set(CMAKE_CXX_FLAGS "-lncurses")