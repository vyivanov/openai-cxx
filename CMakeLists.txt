cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(openaicxx LANGUAGES CXX
    DESCRIPTION "C++ binding to open.ai REST API")

find_package(Boost REQUIRED)
find_package(GTest REQUIRED)    # TODO: Update CMake up to 3.23 in order to find GTest::GMock as well.

set(INC_PRIV inc/priv/fake-connector.hpp
             inc/priv/http-connector.hpp
             inc/priv/mock-connector.hpp
             inc/priv/rest-connector.hpp)

set(SRC src/priv/fake-connector.cpp
        src/priv/http-connector.cpp
        src/priv/mock-connector.cpp)

add_library(${PROJECT_NAME} SHARED ${INC_PRIV} ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE inc/priv/ Boost::headers)
target_link_libraries(${PROJECT_NAME} PRIVATE gmock)
target_compile_options(${PROJECT_NAME} PUBLIC -std=c++20 -Wall -Wextra -Wpedantic -Werror)

enable_testing()
add_subdirectory(tests)
