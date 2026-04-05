cmake_minimum_required(VERSION 3.20)

project(TerminalChessAI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(terminalchess_ai
    src/main.cpp
    src/chess.cpp
    src/player.cpp
    src/path_node.cpp
    src/bot.cpp
)

set_target_properties(terminalchess_ai PROPERTIES OUTPUT_NAME "terminalchess_ai")

target_include_directories(terminalchess_ai PRIVATE include)

if(MSVC)
    target_compile_options(terminalchess_ai PRIVATE /W4 /permissive-)
else()
    target_compile_options(terminalchess_ai PRIVATE -Wall -Wextra -Wpedantic)
endif()

enable_testing()
add_test(NAME smoke_help COMMAND $<TARGET_FILE:terminalchess_ai> --help)
add_test(NAME smoke_version COMMAND $<TARGET_FILE:terminalchess_ai> --version)
