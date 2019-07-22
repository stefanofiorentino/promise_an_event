cmake_minimum_required(VERSION 3.14)
project(promise_an_event)

set(CMAKE_CXX_STANDARD 14)

add_executable(promise_an_event main.cpp)
target_link_libraries(promise_an_event pthread)