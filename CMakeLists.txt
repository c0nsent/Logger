cmake_minimum_required(VERSION 4.0)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(Logger)

add_library(Logger SHARED
        src/Logger.hpp
        src/Logger.cxx)

target_include_directories(Logger PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

