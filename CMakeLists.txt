cmake_minimum_required(VERSION 3.17)
project(MEMOC_project)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}" ${CMAKE_MODULE_PATH})

FIND_PACKAGE(CPLEX REQUIRED)

INCLUDE_DIRECTORIES(include ${CPLEX_INCLUDE_DIR})
add_executable(MEMOC_project main.cpp cpxmacro.h)
TARGET_LINK_LIBRARIES(MEMOC_project  ${CPLEX_LIBRARIES})

