cmake_minimum_required(VERSION 3.16)

project(opencvTest LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenCV REQUIRED COMPONENTS)

add_executable(kt3 main.cpp)

target_include_directories(kt3 PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(kt3 PRIVATE
    ${OpenCV_LIBS}
)
