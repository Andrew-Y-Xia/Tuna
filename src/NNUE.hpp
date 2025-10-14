//
//  NNUE.hpp
//  Tuna Chess Engine
//
//  NNUE (Efficiently Updatable Neural Network) Evaluation
//  Architecture: (768 -> 128)x2 -> 1
//  - Input: 768 features (piece-square inputs, dual perspective)
//  - Hidden: 128 neurons per perspective with SCReLU activation
//  - Output: Single evaluation score
//

#ifndef NNUE_hpp
#define NNUE_hpp

#include "depend.hpp"
#include "Board.hpp"

// Forward declaration to avoid circular dependency
class Board;

namespace NNUE {
    // Network architecture constants
    constexpr int INPUT_SIZE = 768;
    constexpr int HIDDEN_SIZE = 128;
    constexpr int OUTPUT_SIZE = 1;
    
    // Quantization parameters (must match training)
    constexpr int16_t QA = 255;  // Quantization scale for layer 0 weights/bias and output bias
    constexpr int16_t QB = 64;   // Quantization scale for layer 1 weights
    constexpr int32_t QAB = QA * QB;  // Combined scale = 16320
    
    // Evaluation scale (centipawns)
    constexpr int SCALE = 400;
    
    // Network weights (quantized to int16)
    struct alignas(64) Network {
        // Layer 0: Feature transformer (768 -> 128)
        int16_t l0_weights[INPUT_SIZE * HIDDEN_SIZE];
        int16_t l0_bias[HIDDEN_SIZE];
        
        // Layer 1: Output layer (256 -> 1)
        int16_t l1_weights[2 * HIDDEN_SIZE];
        int16_t l1_bias;
    };
    
    // Global network instance
    extern Network network;
    extern bool network_loaded;
    
    // Initialize NNUE from embedded weights (compiled-in)
    bool init_embedded();
    
    // Initialize NNUE from file
    bool init(const std::string& path);
    
    // Load network weights from quantized binary file
    bool load_network(const std::string& path);
    
    // Check if network is loaded
    bool is_loaded();
    
    // Feature indexing for piece-square representation (Chess768 format)
    // Format: 768 features organized as:
    // - White pieces: 0-383 (King=0-63, Queen=64-127, Rook=128-191, Bishop=192-255, Knight=256-319, Pawn=320-383)
    // - Black pieces: 384-767 (King=384-447, Queen=448-511, ..., Pawn=704-767)
    // From STM perspective: piece at its actual position
    // From NTM perspective: vertically flipped (rank flipped)
    inline int feature_index_stm(int piece, int square, int color) {
        // piece: PIECE_KING=2, PIECE_QUEEN=3, ..., PIECE_PAWN=7
        // Convert to 0-based: King=0, Queen=1, Rook=2, Bishop=3, Knight=4, Pawn=5
        int piece_type = piece - 2;
        
        // Chess768 format: color * 384 + piece_type * 64 + square
        return color * 384 + piece_type * 64 + square;
    }
    
    inline int feature_index_ntm(int piece, int square, int color) {
        // piece: PIECE_KING=2, PIECE_QUEEN=3, ..., PIECE_PAWN=7
        int piece_type = piece - 2;
        
        // For NTM perspective: swap colors and flip square vertically
        int ntm_color = 1 - color;
        int flipped_square = square ^ 56;  // Flip rank (XOR with 56)
        
        // Chess768 format: color * 384 + piece_type * 64 + square
        return ntm_color * 384 + piece_type * 64 + flipped_square;
    }
    
    // Non-incremental evaluation (recalculates from scratch)
    // Returns evaluation in centipawns from the perspective of the side to move
    int evaluate(const Board& board);
    
    // SCReLU activation function (Squared Clipped ReLU)
    // SCReLU(x) = min(max(x, 0), 1)^2
    inline int32_t screlu(int32_t x) {
        int32_t clipped = std::max(int32_t(0), std::min(x, int32_t(QA)));
        return clipped * clipped;
    }
}

#endif /* NNUE_hpp */
