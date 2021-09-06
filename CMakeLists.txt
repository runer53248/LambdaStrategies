cmake_minimum_required(VERSION 3.14.0)
project(lambdaStrategy)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_C_FLAGS_DEBUG "-O0")

add_executable(${PROJECT_NAME} main.cpp)
link_libraries(${PROJECT_NAME} -lomp)
