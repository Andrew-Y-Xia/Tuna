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

# Generate embedded NNUE network header at build time
set(NNUE_NETWORK_FILE "${CMAKE_SOURCE_DIR}/NNUE/checkpoints/tuna-100/quantised.bin")
set(NNUE_EMBEDDED_HEADER "${CMAKE_SOURCE_DIR}/src/NNUE_embedded.hpp")
set(EMBED_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/embed_network.py")

# Custom command to generate the embedded header whenever the network file changes
add_custom_command(
    OUTPUT ${NNUE_EMBEDDED_HEADER}
    COMMAND python3 ${EMBED_SCRIPT} ${NNUE_NETWORK_FILE} ${NNUE_EMBEDDED_HEADER}
    DEPENDS ${NNUE_NETWORK_FILE} ${EMBED_SCRIPT}
    COMMENT "Generating embedded NNUE network header from ${NNUE_NETWORK_FILE}"
    VERBATIM
)

# Create a custom target that depends on the generated header
add_custom_target(generate_embedded_network DEPENDS ${NNUE_EMBEDDED_HEADER})

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
        src/NNUE.hpp
        ${NNUE_EMBEDDED_HEADER})

# Make sure the embedded header is generated before building Tuna
add_dependencies(Tuna generate_embedded_network)

# Test executables
# Create a library with common source files to avoid recompilation
add_library(TunaCore STATIC
        src/Bitboard.cpp
        src/Board.cpp
        src/Data_structs.cpp
        src/Evaluation.cpp
        src/Opening_book.cpp
        src/Ray_gen.cpp
        src/Search.cpp
        src/Transposition_table.cpp
        src/Tuning_parameters.cpp
        src/Utility.cpp
        src/Zobrist.cpp
        src/Time_handler.cpp
        src/NNUE.cpp
        ${NNUE_EMBEDDED_HEADER})

add_dependencies(TunaCore generate_embedded_network)

# Test: NNUE Complex Positions
add_executable(test_nnue_complex test_nnue_complex.cpp)
target_link_libraries(test_nnue_complex TunaCore)

# Test: Tuning Parameters
add_executable(test_tuning_params test_tuning_params.cpp)
target_link_libraries(test_tuning_params TunaCore)
