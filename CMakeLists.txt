cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(openaicxx LANGUAGES CXX
    DESCRIPTION "C++ binding to open.ai REST API")

find_package(GTest REQUIRED)
find_package(nlohmann_json REQUIRED CONFIG)

add_executable(${PROJECT_NAME}-main main.cpp)
target_link_libraries(${PROJECT_NAME}-main PRIVATE cpr)     # FIXME: Find 'cpr'
target_compile_options(${PROJECT_NAME}-main PRIVATE         # TODO: Move options to 'openaicxx' itself
    -std=c++20
    -Wall -Wextra -Wpedantic)
