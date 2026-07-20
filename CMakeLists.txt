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
    src/explain.c
    src/history.c
    src/aliases.c
    src/native_api/file_ops.c
)

if (WIN32)
    target_sources(smart_terminal_core PRIVATE src/native_api/windows_file_ops.c)
else()
    target_sources(smart_terminal_core PRIVATE src/native_api/posix_file_ops.c)
endif()

target_include_directories(smart_terminal_core PUBLIC src)

# Build main executable
add_executable(smart_terminal src/main.c)
target_link_libraries(smart_terminal smart_terminal_core)

option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(ENABLE_SANITIZERS "Enable ASAN and UBSAN" OFF)

if(ENABLE_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()

if(ENABLE_SANITIZERS)
    # Check if compiler supports these flags (usually GCC/Clang)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address,undefined -g -fno-omit-frame-pointer")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")
    endif()
endif()

# Testing
enable_testing()

set(TEST_FRAMEWORK_SRC tests/test_framework.c)

macro(add_cmdbridge_test TEST_NAME SRC_FILE)
    add_executable(${TEST_NAME} ${SRC_FILE} ${TEST_FRAMEWORK_SRC})
    target_link_libraries(${TEST_NAME} smart_terminal_core)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endmacro()

add_cmdbridge_test(test_parser tests/test_parser.c)
add_cmdbridge_test(test_mapper tests/test_mapper.c)
add_cmdbridge_test(test_safety tests/test_safety.c)
add_cmdbridge_test(test_native_api tests/test_native_api.c)
add_cmdbridge_test(test_utils tests/test_utils.c)
add_cmdbridge_test(integration_tests tests/integration_tests.c)
add_cmdbridge_test(security_tests tests/security_tests.c)
add_cmdbridge_test(fuzz_tests tests/fuzz_tests.c)

# Installation support
install(TARGETS smart_terminal DESTINATION bin)
install(DIRECTORY config DESTINATION bin)
install(DIRECTORY data DESTINATION bin)
