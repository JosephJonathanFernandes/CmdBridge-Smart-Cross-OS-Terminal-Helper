cmake_minimum_required(VERSION 3.10)
project(SmartTerminal C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add source files
set(SOURCES
    src/main.c
    src/parser.c
    src/intent.c
    src/os_mapper.c
    src/safety.c
    src/commands.c
    src/utils.c
    src/logger.c
)

# Core library (so we can link tests against it without main.c)
add_library(smart_terminal_core STATIC
    src/parser.c
    src/intent.c
    src/os_mapper.c
    src/safety.c
    src/commands.c
    src/utils.c
    src/logger.c
)
target_include_directories(smart_terminal_core PUBLIC src)

# Build main executable
add_executable(smart_terminal src/main.c)
target_link_libraries(smart_terminal smart_terminal_core)

# Testing
enable_testing()

add_executable(test_parser tests/test_parser.c)
target_link_libraries(test_parser smart_terminal_core)
add_test(NAME parser_tests COMMAND test_parser)

add_executable(test_safety tests/test_safety.c)
target_link_libraries(test_safety smart_terminal_core)
add_test(NAME safety_tests COMMAND test_safety)
