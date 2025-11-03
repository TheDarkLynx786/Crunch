cmake_minimum_required(VERSION 3.10)

project(Crunch)

# Locate LLVM
find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

# Include LLVM headers
include_directories(${LLVM_INCLUDE_DIRS})
message(STATUS "LLVM include dirs: ${LLVM_INCLUDE_DIRS}")
add_definitions(${LLVM_DEFINITIONS})

add_executable(CrunchRunner 
    src/main.cpp
    src/lexer/lexer.cpp
    src/parser/parser.cpp
    src/ast/ast.cpp
    src/semantics/symbol_table.cpp
)

# Link LLVM libraries
llvm_map_components_to_libnames(llvm_libs core irreader support analysis)

target_link_libraries(CrunchRunner
    ${llvm_libs}
)

set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 
set(CMAKE_CXX_EXTENSIONS OFF)