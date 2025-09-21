cmake_minimum_required(VERSION 3.10)

project(Crunch)

add_executable(CrunchRunner 
    src/main.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/ast/ast/cpp
)


set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF)