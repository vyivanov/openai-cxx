cmake_minimum_required(VERSION 3.18 FATAL_ERROR)
project(openaicxx LANGUAGES CXX
    DESCRIPTION "C++ binding to open.ai REST API")

set(INC_PRIV inc/priv/rest-connector.hpp)
