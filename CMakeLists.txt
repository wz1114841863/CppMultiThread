cmake_minimum_required(VERSION 3.19)
project(multiThread)

set(CMAKE_CXX_STANDARD 14)

add_executable(multiThread main.cpp)

add_subdirectory(chapter01)
add_subdirectory(chapter02)
add_subdirectory(chapter03)
target_link_libraries(multiThread PRIVATE chap01)
target_link_libraries(multiThread PRIVATE chap02)
target_link_libraries(multiThread PRIVATE chap03)