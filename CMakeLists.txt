cmake_minimum_required(VERSION 3.16)

set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

project(ipc-boost)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/dependencies.conan.cmake)

find_package(Boost REQUIRED)

add_subdirectory(client)
add_subdirectory(server)
