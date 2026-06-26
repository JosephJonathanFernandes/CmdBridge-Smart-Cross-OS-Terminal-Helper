cmake_minimum_required(VERSION 3.10)
project(SmartTerminal C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Add source files
set(SOURCES
    main.c
    parser.c
    intent.c
    os_mapper.c
    safety.c
    commands.c
    utils.c
)

# Build executable
add_executable(smart_terminal ${SOURCES})
