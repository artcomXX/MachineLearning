cmake_minimum_required(VERSION 3.10)
project(kt1a)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(kt1a main.cpp)


target_link_libraries(kt1a ${OpenCV_LIBS})
