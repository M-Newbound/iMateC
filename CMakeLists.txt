cmake_minimum_required(VERSION 3.10)

project(iMateC)

# Set the output directory for the build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Add the main.c file to the project
add_executable(iMateC src/main.c)

# Add a custom target to clean the build directory
add_custom_target(clean_build
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    COMMENT "Cleaning build directory"
)

# Make the clean_build target depend on the build target
add_dependencies(clean_build iMateC)