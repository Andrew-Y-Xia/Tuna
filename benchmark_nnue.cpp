//
//  benchmark_nnue.cpp
//  Benchmark NNUE evaluation performance
//

#include "src/Board.hpp"
#include "src/NNUE.hpp"
#include "src/Evaluation.hpp"
#include <iostream>
#include <chrono>
#include <vector>

using namespace std::chrono;

// Benchmark parameters
const int WARMUP_ITERATIONS = 1000;
const int BENCHMARK_ITERATIONS = 100000;

struct BenchmarkResult {
    std::string name;
    double avg_time_ns;
    long long total_ops;
};

void benchmark_evaluation(Board& board, std::vector<BenchmarkResult>& results) {
    auto* acc = board.get_nnue_accumulator();
    
    // Warmup
    for (int i = 0; i < WARMUP_ITERATIONS; i++) {
        volatile int eval = NNUE::evaluate_incremental(*acc, board.get_current_turn());
        (void)eval;
    }
    
    // Benchmark incremental evaluation
    auto start = high_resolution_clock::now();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        volatile int eval = NNUE::evaluate_incremental(*acc, board.get_current_turn());
        (void)eval;
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(end - start).count();
    
    results.push_back({
        "Incremental Evaluation",
        static_cast<double>(duration) / BENCHMARK_ITERATIONS,
        BENCHMARK_ITERATIONS
    });
    
    // Benchmark from-scratch evaluation
    start = high_resolution_clock::now();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        volatile int eval = NNUE::evaluate(board);
        (void)eval;
    }
    end = high_resolution_clock::now();
    duration = duration_cast<nanoseconds>(end - start).count();
    
    results.push_back({
        "From-Scratch Evaluation",
        static_cast<double>(duration) / BENCHMARK_ITERATIONS,
        BENCHMARK_ITERATIONS
    });
}

void benchmark_accumulator_update(Board& board, std::vector<BenchmarkResult>& results) {
    MoveList moves;
    board.generate_moves(moves);
    
    if (moves.begin() == moves.end()) {
        std::cout << "No moves available for benchmark" << std::endl;
        return;
    }
    
    // Warmup
    for (int i = 0; i < WARMUP_ITERATIONS; i++) {
        auto move = *moves.begin();
        board.make_move(move);
        board.unmake_move();
    }
    
    // Benchmark make_move (includes accumulator update)
    auto start = high_resolution_clock::now();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        auto move = *moves.begin();
        board.make_move(move);
        board.unmake_move();
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(end - start).count();
    
    results.push_back({
        "Make/Unmake Move (with NNUE update)",
        static_cast<double>(duration) / BENCHMARK_ITERATIONS,
        BENCHMARK_ITERATIONS
    });
}

void benchmark_accumulator_refresh(Board& board, std::vector<BenchmarkResult>& results) {
    NNUE::Accumulator acc;
    
    // Warmup
    for (int i = 0; i < WARMUP_ITERATIONS; i++) {
        NNUE::refresh_accumulator(acc, board);
    }
    
    // Benchmark refresh
    auto start = high_resolution_clock::now();
    for (int i = 0; i < BENCHMARK_ITERATIONS; i++) {
        NNUE::refresh_accumulator(acc, board);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<nanoseconds>(end - start).count();
    
    results.push_back({
        "Accumulator Refresh",
        static_cast<double>(duration) / BENCHMARK_ITERATIONS,
        BENCHMARK_ITERATIONS
    });
}

int main() {
    // Initialize
    init_eval_utils();
    init_bitboard_utils();
    
    std::string nnue_path = "NNUE/checkpoints/tuna-100/quantised.bin";
    if (!NNUE::init(nnue_path)) {
        std::cerr << "Failed to load NNUE network!" << std::endl;
        return 1;
    }
    
    std::cout << "=== NNUE Performance Benchmark ===" << std::endl;
    std::cout << std::endl;
    
    // Test positions
    std::vector<std::string> positions = {
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",  // Starting
        "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",  // Kiwipete
        "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",  // Endgame
    };
    
    std::vector<std::string> position_names = {
        "Starting Position",
        "Kiwipete (Complex Middle)",
        "Endgame Position"
    };
    
    for (size_t pos_idx = 0; pos_idx < positions.size(); pos_idx++) {
        Board board;
        board.read_FEN(positions[pos_idx]);
        
        std::cout << "Position: " << position_names[pos_idx] << std::endl;
        std::cout << std::string(70, '-') << std::endl;
        
        std::vector<BenchmarkResult> results;
        
        benchmark_evaluation(board, results);
        benchmark_accumulator_update(board, results);
        benchmark_accumulator_refresh(board, results);
        
        // Print results
        for (const auto& result : results) {
            double time_us = result.avg_time_ns / 1000.0;
            double ops_per_sec = 1e9 / result.avg_time_ns;
            
            std::cout << result.name << ":" << std::endl;
            std::cout << "  Average time: " << time_us << " μs (" 
                      << result.avg_time_ns << " ns)" << std::endl;
            std::cout << "  Throughput: " << ops_per_sec << " ops/sec" << std::endl;
            std::cout << std::endl;
        }
        
        std::cout << std::endl;
    }
    
    // System info
    std::cout << "=== System Information ===" << std::endl;
    std::cout << "Compiler: " << __VERSION__ << std::endl;
    #ifdef __AVX2__
        std::cout << "AVX2: Enabled" << std::endl;
    #else
        std::cout << "AVX2: Not enabled" << std::endl;
    #endif
    #ifdef __AVX__
        std::cout << "AVX: Enabled" << std::endl;
    #else
        std::cout << "AVX: Not enabled" << std::endl;
    #endif
    #ifdef __SSE4_1__
        std::cout << "SSE4.1: Enabled" << std::endl;
    #else
        std::cout << "SSE4.1: Not enabled" << std::endl;
    #endif
    
    return 0;
}
