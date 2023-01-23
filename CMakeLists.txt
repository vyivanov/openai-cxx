cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(openaicxx LANGUAGES CXX
    DESCRIPTION "C++ binding to open.ai REST API")

find_package(Boost REQUIRED)
find_package(GTest REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
# TODO: Update CMake up to 3.23 in order to find GTest::GMock as well.
# TODO: Add 'cpr' via find_package(...).

set(INC_PUB inc/pub/completion.hpp)

set(INC_PRIV inc/priv/completion-service.hpp
             inc/priv/fake-connector.hpp
             inc/priv/http-connector.hpp
             inc/priv/make.hpp
             inc/priv/mock-connector.hpp
             inc/priv/rest-connector.hpp
             inc/priv/ser-des.hpp)

set(SRC src/priv/completion-service.cpp
        src/priv/fake-connector.cpp
        src/priv/http-connector.cpp
        src/priv/mock-connector.cpp
        src/pub/completion.cpp)

add_library(${PROJECT_NAME} SHARED ${INC_PUB} ${INC_PRIV} ${SRC})
target_include_directories(${PROJECT_NAME} PRIVATE inc/priv/ Boost::headers)
target_include_directories(${PROJECT_NAME} PUBLIC inc/pub)
target_link_libraries(${PROJECT_NAME} PRIVATE cpr gmock)
target_compile_options(${PROJECT_NAME} PUBLIC -std=c++20 -Wall -Wextra -Wpedantic -Werror)

enable_testing()
add_subdirectory(tests)
