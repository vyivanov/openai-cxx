cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(openaicxx LANGUAGES CXX
    DESCRIPTION "C++ binding to open.ai REST API")

set(INC_PRIV inc/priv/fake-connector.hpp
             inc/priv/rest-connector.hpp)

set(SRC src/priv/fake-connector.cpp)

add_library(${PROJECT_NAME} SHARED ${INC_PRIV} ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE inc/priv/)

target_compile_options(${PROJECT_NAME} PUBLIC
    -std=c++20
    -Wall
    -Wextra
    -Wpedantic
    -Werror)
