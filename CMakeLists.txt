# iMate -- Copyright (C) 2024 Martin Newbound

cmake_minimum_required(VERSION 3.10)
project(iMateC)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Default to Release so the engine actually runs at speed.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ---------------------------------------------------------------------------
# Core library - everything except Main.c (which owns main())
# ---------------------------------------------------------------------------
file(GLOB_RECURSE ALL_SOURCES "src/*.c")
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*Main\\.c$")

add_library(iMateCore OBJECT ${ALL_SOURCES})
target_include_directories(iMateCore PUBLIC src)
target_compile_options(iMateCore PRIVATE -Wall -Wextra -Wpedantic)

# ---------------------------------------------------------------------------
# Engine executable
# ---------------------------------------------------------------------------
add_executable(iMateC src/Main.c)
target_link_libraries(iMateC PRIVATE iMateCore)
target_compile_options(iMateC PRIVATE -Wall -Wextra -Wpedantic)

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
enable_testing()

set(TEST_SOURCES
    tests/test_gamestate.c
    tests/test_move_generation.c
    tests/test_evaluation.c
    tests/test_magic_bitboards.c
    tests/test_perft.c
)

foreach(TEST_SRC ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SRC})
    target_link_libraries(${TEST_NAME} PRIVATE iMateCore m)
    target_include_directories(${TEST_NAME} PRIVATE src tests)
    target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -Wpedantic)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()
