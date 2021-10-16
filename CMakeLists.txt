cmake_minimum_required(VERSION 3.5)
project(ivy)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(IVY_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(client)
add_subdirectory(server)
