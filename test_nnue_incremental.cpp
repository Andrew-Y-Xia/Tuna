//
//  test_nnue_incremental.cpp
//  Test NNUE incremental accumulator updates
//
//  This test uses perft to verify that incremental NNUE evaluation
//  matches the from-scratch evaluation at every node of the search tree.
//

#include "src/Board.hpp"
#include "src/NNUE.hpp"
#include "src/Evaluation.hpp"
#include "src/Data_structs.hpp"
#include <iostream>
#include <string>

// Counter for mismatches
int mismatches = 0;
int nodes_tested = 0;


// Perft with NNUE accumulator verification
long perft_nnue_test(Board& board, unsigned int depth) {
    if (depth == 0) {
        nodes_tested++;
        // Compare incremental eval with from-scratch eval
        auto* acc = board.get_nnue_accumulator();
        if (NNUE::is_loaded() && acc) {
            int incremental_eval = NNUE::evaluate_incremental(*acc, board.get_current_turn());
            int from_scratch_eval = NNUE::evaluate(board);
            
            if (incremental_eval != from_scratch_eval) {
                mismatches++;
                if (mismatches == 1) {  // Print details for first mismatch only
                    std::cout << "FIRST MISMATCH at node " << nodes_tested << ": "
                              << "Incremental=" << incremental_eval 
                              << " FromScratch=" << from_scratch_eval 
                              << " Diff=" << (incremental_eval - from_scratch_eval) << std::endl;
                    std::cout << "Board state:" << std::endl;
                    board.print_board();
                } else if (mismatches <= 10) {
                    std::cout << "MISMATCH at node " << nodes_tested << ": "
                              << "Incremental=" << incremental_eval 
                              << " FromScratch=" << from_scratch_eval 
                              << " Diff=" << (incremental_eval - from_scratch_eval) << std::endl;
                }
            }
        }
        
        return 1;
    }

    long nodes = 0;
    MoveList moves;
    board.generate_moves(moves);

    for (auto it = moves.begin(); it != moves.end(); ++it) {
        board.make_move(*it);
        nodes += perft_nnue_test(board, depth - 1);
        board.unmake_move();
    }
    
    return nodes;
}

int main() {
    // Initialize evaluation utilities
    init_eval_utils();
    init_bitboard_utils();
    
    // Load NNUE network
    std::string nnue_path = "NNUE/checkpoints/tuna-100/quantised.bin";
    
    std::cout << "Loading NNUE network from: " << nnue_path << std::endl;
    
    if (!NNUE::init(nnue_path)) {
        std::cerr << "Failed to load NNUE network!" << std::endl;
        return 1;
    }
    
    std::cout << "NNUE network loaded successfully!\n" << std::endl;
    
    // Test positions
    struct TestPosition {
        std::string name;
        std::string fen;
        int depth;
    };

    std::vector<TestPosition> positions = {
        {"Starting position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 5},
        {"Kiwipete", "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 5},
        {"Position 3", "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 5},
        {"Position 4", "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 5},
        {"Position 5", "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 5},
        {"Endgame", "8/8/4k3/8/8/4K3/4P3/8 w - - 0 1", 6},
    };
    
    std::cout << "Testing NNUE incremental accumulator updates:\n" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    bool all_passed = true;
    
    for (const auto& test : positions) {
        mismatches = 0;
        nodes_tested = 0;
        
        std::cout << "\nTesting: " << test.name << std::endl;
        std::cout << "FEN: " << test.fen << std::endl;
        std::cout << "Depth: " << test.depth << std::endl;
        
        Board board(test.fen);
        board.refresh_nnue_accumulator();  // Refresh after network is loaded
        
        long nodes = perft_nnue_test(board, test.depth);
        
        std::cout << "Nodes: " << nodes << std::endl;
        std::cout << "Nodes tested: " << nodes_tested << std::endl;
        
        if (mismatches == 0) {
            std::cout << "✓ PASSED - All evaluations match!" << std::endl;
        } else {
            std::cout << "✗ FAILED - " << mismatches << " mismatches found" << std::endl;
            all_passed = false;
        }
        
        std::cout << std::string(80, '-') << std::endl;
    }
    
    std::cout << "\n" << std::string(80, '=') << std::endl;
    if (all_passed) {
        std::cout << "SUCCESS: All tests passed! Incremental updates are correct." << std::endl;
    } else {
        std::cout << "FAILURE: Some tests failed. Accumulator updates have bugs." << std::endl;
    }

    
    return 0;
}
