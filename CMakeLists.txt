cmake_minimum_required(VERSION 3.16)

project(Dijkstra)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

add_executable(GraphTest 
                GraphTest.cpp
                Graph.h
                Dijkstra.h)

add_test(NAME BigTest 
         COMMAND GraphTest ${CMAKE_CURRENT_SOURCE_DIR}/Dijkstra.txt)