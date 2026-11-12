cmake_minimum_required(VERSION 3.16)

project(kt4b LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED COMPONENTS)

add_executable(kt5 main.cpp)

target_include_directories(kt5 PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(kt5 PRIVATE
    ${OpenCV_LIBS}
)
