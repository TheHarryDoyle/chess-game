cmake_minimum_required(VERSION 3.16)
project(ChessPhase2 VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CHESS_ENABLE_GUI "Build the Qt board interface" ON)

add_library(chess_core
    src/Board.cpp
    src/MoveValidator.cpp
    src/ChessEngine.cpp
)

target_include_directories(chess_core PUBLIC include)

target_compile_options(chess_core PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)

enable_testing()

add_executable(chess_tests tests/test_board.cpp)
target_link_libraries(chess_tests PRIVATE chess_core)
add_test(NAME chess_tests COMMAND chess_tests)

if(CHESS_ENABLE_GUI)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)

    qt_standard_project_setup()

    qt_add_executable(chess_app
        src/gui/main.cpp
        src/gui/ChessWindow.cpp
        include/chess/ChessWindow.h
    )

    target_link_libraries(chess_app PRIVATE chess_core Qt6::Widgets)
    target_include_directories(chess_app PRIVATE include)
endif()
