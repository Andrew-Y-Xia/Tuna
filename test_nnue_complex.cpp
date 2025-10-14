//
//  test_nnue_complex.cpp
//  Test NNUE evaluation on complex positions
//

#include "src/Board.hpp"
#include "src/NNUE.hpp"
#include "src/Evaluation.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

int main() {
    // Initialize evaluation utilities
    init_eval_utils();
    
    // Load NNUE network
    std::string nnue_path = "NNUE/checkpoints/tuna-60/quantised.bin";
    
    std::cout << "Loading NNUE network from: " << nnue_path << std::endl;
    
    if (!NNUE::init(nnue_path)) {
        std::cerr << "Failed to load NNUE network!" << std::endl;
        return 1;
    }
    
    std::cout << "NNUE network loaded successfully!\n" << std::endl;
    
    // Complex test positions
    std::vector<std::pair<std::string, std::string>> test_positions = {
        {"Starting position", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"},
        
        // Material imbalances
        {"White up a pawn", "rnbqkbnr/pppp1ppp/8/4p3/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1"},
        {"White up a knight", "rnbqkb1r/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"},
        {"White up a rook", "rnbqkbn1/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"},
        {"White up a queen", "rnb1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"},
        {"Black up a pawn", "rnbqkbnr/pppppppp/8/8/8/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1"},
        {"Black up a knight", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/R1BQKBNR w KQkq - 0 1"},
        
        // Famous openings
        {"Ruy Lopez", "r1bqkbnr/pppp1ppp/2n5/1B2p3/4P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 3 3"},
        {"Sicilian Defense", "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"},
        {"French Defense", "rnbqkbnr/pppp1ppp/4p3/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2"},
        {"Caro-Kann Defense", "rnbqkbnr/pp1ppppp/2p5/8/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 2"},
        {"Italian Game", "r1bqkb1r/pppp1ppp/2n2n2/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 4 4"},
        {"Queen's Gambit", "rnbqkbnr/ppp1pppp/8/3p4/2PP4/8/PP2PPPP/RNBQKBNR b KQkq c3 0 2"},
        
        // Tactical positions
        {"Scholar's Mate threat", "r1bqkb1r/pppp1ppp/2n2n2/4p2Q/2B1P3/8/PPPP1PPP/RNB1K1NR b KQkq - 4 4"},
        {"Back rank mate threat", "6k1/5ppp/8/8/8/8/5PPP/R5K1 w - - 0 1"},
        
        // Endgames
        {"King and pawn vs King", "8/8/8/4k3/8/8/4P3/4K3 w - - 0 1"},
        {"Rook endgame", "8/5k2/8/8/8/8/3R4/4K3 w - - 0 1"},
        {"Queen endgame", "8/5k2/8/8/8/8/3Q4/4K3 w - - 0 1"},
        {"Opposite colored bishops", "8/5k2/3b4/8/8/3B4/5K2/8 w - - 0 1"},
        
        // Different side to move
        {"Starting (black to move)", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR b KQkq - 0 1"},
        {"Italian Game (black to move)", "r1bqkb1r/pppp1ppp/2n2n2/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R b KQkq - 4 4"},
        
        // Castling scenarios
        {"Both sides can castle", "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1"},
        {"After white castles kingside", "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R4RK1 w kq - 1 2"},
        {"After white castles queenside", "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/2KR3R w kq - 1 2"},
    };
    
    std::cout << "Testing NNUE evaluation on complex positions:\n" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    for (const auto& [description, fen] : test_positions) {
        Board board(fen);
        
        int nnue_eval = nnue_evaluate(board);
        int trad_eval = board.static_eval();
        
        std::cout << description << std::endl;
        std::cout << "  FEN: " << fen << std::endl;
        std::cout << "  NNUE: " << std::setw(5) << nnue_eval << " cp"
                  << "  |  Traditional: " << std::setw(5) << trad_eval << " cp"
                  << "  |  Diff: " << std::setw(5) << (nnue_eval - trad_eval) << " cp"
                  << std::endl;
        std::cout << std::string(80, '-') << std::endl;
    }
    
    std::cout << "\nNNUE complex position test complete!" << std::endl;
    
    return 0;
}
