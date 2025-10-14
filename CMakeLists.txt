cmake_minimum_required(VERSION 3.20)
project(Tuna)

set(CMAKE_CXX_STANDARD 14)

FILE(COPY Resources DESTINATION "${CMAKE_BINARY_DIR}")

add_compile_options(
    -Wall  
    $<$<CONFIG:RELEASE>:-O3> $<$<CONFIG:RELEASE>:-flto> $<$<CONFIG:RELEASE>:-march=native>
    $<$<CONFIG:DEBUG>:-O1>
    $<$<CONFIG:DEBUG>:-ggdb3>
)
add_compile_definitions(
    $<$<CONFIG:RELEASE>:NDEBUG>
    $<$<CONFIG:RELEASE>:BOOST_DISABLE_ASSERTS>
)

include_directories(src)

add_executable(Tuna
        src/Bitboard.cpp
        src/Bitboard.hpp
        src/Board.cpp
        src/Board.hpp
        src/Data_structs.cpp
        src/Data_structs.hpp
        src/depend.hpp
        src/Engine.cpp
        src/Engine.hpp
        src/Evaluation.cpp
        src/Evaluation.hpp
        src/main.cpp
        src/Opening_book.cpp
        src/Opening_book.hpp
        src/Ray_gen.cpp
        src/Ray_gen.hpp
        src/Search.cpp
        src/Search.hpp
        src/Thread.cpp
        src/Thread.hpp
        src/Transposition_table.cpp
        src/Transposition_table.hpp
        src/Tuning_parameters.cpp
        src/Tuning_parameters.hpp
        src/UCI.cpp
        src/UCI.hpp
        src/Utility.cpp
        src/Utility.hpp
        src/Zobrist.cpp
        src/Zobrist.hpp 
        src/Time_handler.cpp 
        src/Time_handler.hpp 
        src/tests.cpp 
        src/tests.hpp
        src/NNUE.cpp
        src/NNUE.hpp)
