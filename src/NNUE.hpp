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
    
    // Enable/disable incremental updates (set to false to always recalculate from scratch)
    constexpr bool USE_INCREMENTAL = true;
    
    // Accumulator structure for incremental updates
    struct Accumulator {
        int32_t white_hidden[HIDDEN_SIZE];  // White's perspective accumulator
        int32_t black_hidden[HIDDEN_SIZE];  // Black's perspective accumulator
        
        // Cached evaluation results (avoid recomputing if accumulator unchanged)
        int16_t cached_eval_white;  // Cached eval when white to move
        int16_t cached_eval_black;  // Cached eval when black to move
        bool white_cache_valid;     // Is white cache valid?
        bool black_cache_valid;     // Is black cache valid?
        
        // Constructor to initialize cache as invalid
        Accumulator() : cached_eval_white(0), cached_eval_black(0), 
                       white_cache_valid(false), black_cache_valid(false) {}
    };
    
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
    // Format: 768 features = 12 piece types × 64 squares
    // Piece types: White P,N,B,R,Q,K (0-5), Black p,n,b,r,q,k (6-11)
    // Feature index = piece_type * 64 + square
    // 
    // White Pawn: 0-63, White Knight: 64-127, White Bishop: 128-191,
    // White Rook: 192-255, White Queen: 256-319, White King: 320-383,
    // Black Pawn: 384-447, Black Knight: 448-511, Black Bishop: 512-575,
    // Black Rook: 576-639, Black Queen: 640-703, Black King: 704-767
    //
    // Dual perspective:
    // - From white's perspective: white pieces at actual squares, black pieces flipped
    // - From black's perspective: black pieces at actual squares, white pieces flipped
    // Square numbering: 0=a1, 1=b1, ..., 7=h1, 8=a2, ..., 63=h8
    
    // Non-incremental evaluation (recalculates from scratch)
    // Returns evaluation in centipawns from the perspective of the side to move
    int evaluate(const Board& board);
    
    // Incremental evaluation using pre-computed accumulator
    // Returns evaluation in centipawns from the perspective of the side to move
    // NOTE: Takes non-const reference to cache evaluation results
    int evaluate_incremental(Accumulator& acc, int side_to_move);
    
    // Refresh accumulator from scratch by computing all active features
    void refresh_accumulator(Accumulator& acc, const Board& board);
    
    // Get feature index for a piece on a square from a given perspective
    // perspective: 0 = white's view, 1 = black's view
    int get_feature_index(int piece, int square, int perspective);
    
    // Update accumulator for piece movement (handles both perspectives)
    // piece: Board piece type (2-7), square: 0-63, color: 0=white 1=black
    void add_piece_to_accumulator(Accumulator& acc, int piece, int square, int color);
    void remove_piece_from_accumulator(Accumulator& acc, int piece, int square, int color);
    
    // Invalidate cached evaluation (call after updating accumulator)
    inline void invalidate_cache(Accumulator& acc) {
        acc.white_cache_valid = false;
        acc.black_cache_valid = false;
    }
    
    // SCReLU activation function (Squared Clipped ReLU)
    // SCReLU(x) = min(max(x, 0), 1)^2
    inline int32_t screlu(int32_t x) {
        int32_t clipped = std::max(int32_t(0), std::min(x, int32_t(QA)));
        return clipped * clipped;
    }
}

#endif /* NNUE_hpp */
