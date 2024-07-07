# iMate -- Copyright (C) 2024 Martin Newbound */                                                    

cmake_minimum_required(VERSION 3.10)

project(iMateC)

# Set the output directory for the build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Set the language to C
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Get all source files in the 'src' directory and its subdirectories
file(GLOB_RECURSE SOURCES "src/*.c")

# Add executable with all source files
add_executable(iMateC ${SOURCES})

# Add a custom target to clean the build directory
add_custom_target(clean_build
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT "Cleaning build directory"
)

# Make the clean_build target depend on the build target
add_dependencies(clean_build iMateC)