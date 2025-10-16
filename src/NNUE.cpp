//
//  NNUE.cpp
//  Tuna Chess Engine
//
//  NNUE (Efficiently Updatable Neural Network) Evaluation Implementation
//

#include "NNUE.hpp"
#include "NNUE_embedded.hpp"
#include "Board.hpp"
#include <fstream>
#include <iostream>
#include <cstring>

namespace NNUE {
    Network network;
    bool network_loaded = false;
    
    bool init_embedded() {
        // Copy embedded weights into network structure
        std::memcpy(network.l0_weights, Embedded::l0_weights, 
                    INPUT_SIZE * HIDDEN_SIZE * sizeof(int16_t));
        std::memcpy(network.l0_bias, Embedded::l0_bias, 
                    HIDDEN_SIZE * sizeof(int16_t));
        std::memcpy(network.l1_weights, Embedded::l1_weights, 
                    2 * HIDDEN_SIZE * sizeof(int16_t));
        network.l1_bias = Embedded::l1_bias;
        
        network_loaded = true;
        std::cout << "Successfully loaded embedded NNUE network" << std::endl;
        return true;
    }
    
    bool init(const std::string& path) {
        // If path is empty or default, try to use embedded network first
        if (path.empty() || path == "embedded") {
            return init_embedded();
        }
        
        // Otherwise try to load from file
        bool success = load_network(path);
        
        // If file loading fails, fall back to embedded
        if (!success) {
            std::cout << "Failed to load network from file, trying embedded network..." << std::endl;
            return init_embedded();
        }
        
        return success;
    }
    
    bool load_network(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        
        if (!file.is_open()) {
            std::cerr << "Failed to open NNUE network file: " << path << std::endl;
            return false;
        }
        
        // Read Layer 0 weights (768 * 128 * 2 bytes = 196608 bytes)
        file.read(reinterpret_cast<char*>(network.l0_weights), 
                  INPUT_SIZE * HIDDEN_SIZE * sizeof(int16_t));
        
        // Read Layer 0 bias (128 * 2 bytes = 256 bytes)
        file.read(reinterpret_cast<char*>(network.l0_bias), 
                  HIDDEN_SIZE * sizeof(int16_t));
        
        // Read Layer 1 weights (256 * 2 bytes = 512 bytes)
        file.read(reinterpret_cast<char*>(network.l1_weights), 
                  2 * HIDDEN_SIZE * sizeof(int16_t));
        
        // Read Layer 1 bias (1 * 2 bytes = 2 bytes)
        file.read(reinterpret_cast<char*>(&network.l1_bias), sizeof(int16_t));
        
        if (!file) {
            std::cerr << "Error reading NNUE network file: " << path << std::endl;
            file.close();
            return false;
        }
        
        file.close();
        network_loaded = true;
        
        std::cout << "Successfully loaded NNUE network from: " << path << std::endl;
        return true;
    }
    
    bool is_loaded() {
        return network_loaded;
    }

    // Helper function to convert board piece and color to Chess768 format
    int convert_to_chess768_piece_type(int piece, int color) {
        switch (piece) {
            case 7:  // PIECE_PAWN
                return color == 0 ? 0 : 6;
            case 6:  // PIECE_KNIGHT
                return color == 0 ? 1 : 7;
            case 5:  // PIECE_BISHOP
                return color == 0 ? 2 : 8;
            case 4:  // PIECE_ROOK
                return color == 0 ? 3 : 9;
            case 3:  // PIECE_QUEEN
                return color == 0 ? 4 : 10;
            case 2:  // PIECE_KING
                return color == 0 ? 5 : 11;
            default:
                return -1;  // Invalid piece
        }
    }
    
    int evaluate(const Board& board) {
        if (!network_loaded) {
            std::cerr << "NNUE network not loaded!" << std::endl;
            return 0;
        }
        
        // Get board state
        int current_turn = board.get_current_turn();  // 0 = white, 1 = black
        
        // Initialize hidden layer activations for both color perspectives
        int32_t white_hidden[HIDDEN_SIZE];  // White's perspective
        int32_t black_hidden[HIDDEN_SIZE];  // Black's perspective
        
        // Initialize with biases
        for (int i = 0; i < HIDDEN_SIZE; i++) {
            white_hidden[i] = network.l0_bias[i];
            black_hidden[i] = network.l0_bias[i];
        }
        
        // Accumulate features for all pieces
        // In Chess768 format:
        // - From white's perspective: white pieces unflipped (0-383), black pieces flipped (384-767)
        // - From black's perspective: black pieces unflipped (0-383), white pieces flipped (384-767)
        for (int square = 0; square < 64; square++) {
            unsigned int piece = board.find_piece_occupying_sq(square);
            
            if (piece == PIECE_NONE || piece == PIECE_EXTRA) {
                continue;
            }
            
            // Determine piece color (0 = white, 1 = black)
            int color = board.is_white_piece(square) ? 0 : 1;

            // Convert piece type to Chess768 format (0-11)
            int piece_type = convert_to_chess768_piece_type(piece, color);
            if (piece_type == -1) {
                continue; // Invalid piece
            }

            // Chess768: each perspective sees their OWN pieces at their actual squares,
            // and OPPONENT pieces flipped vertically (square ^ 56)
            // Feature index: piece_type * 64 + square
            
            // For White's perspective:
            //   - White pieces (types 0-5): square as-is (not flipped)
            //   - Black pieces (types 6-11): square ^ 56 (flipped)
            // For Black's perspective:
            //   - Black pieces (types 6-11 become 0-5): square as-is (not flipped)  
            //   - White pieces (types 0-5 become 6-11): square ^ 56 (flipped)
            
            int white_feature, black_feature;
            
            // From white's perspective
            white_feature = piece_type * 64 + square;
            
            // From black's perspective: flip square and swap piece colors
            int flipped_sq = square ^ 56;
            int flipped_piece_type;
            if (piece_type < 6) {
                flipped_piece_type = piece_type + 6;  // White piece -> Black piece
            } else {
                flipped_piece_type = piece_type - 6;  // Black piece -> White piece
            }
            black_feature = flipped_piece_type * 64 + flipped_sq;
            
            // Add this feature's contribution to the hidden layer
            for (int i = 0; i < HIDDEN_SIZE; i++) {
                white_hidden[i] += network.l0_weights[white_feature * HIDDEN_SIZE + i];
                black_hidden[i] += network.l0_weights[black_feature * HIDDEN_SIZE + i];
            }
        }
        
        // Apply SCReLU activation to hidden layers
        int32_t white_activated[HIDDEN_SIZE];
        int32_t black_activated[HIDDEN_SIZE];
        
        for (int i = 0; i < HIDDEN_SIZE; i++) {
            white_activated[i] = screlu(white_hidden[i]);
            black_activated[i] = screlu(black_hidden[i]);
        }
        
        // Compute output layer (concatenate both perspectives)
        // The network always expects: STM perspective, then NTM perspective
        // So if white to move: white_activated first, black_activated second
        // If black to move: black_activated first, white_activated second
        int32_t output = network.l1_bias;
        
        if (current_turn == 0) {  // White to move
            // STM = white, NTM = black
            for (int i = 0; i < HIDDEN_SIZE; i++) {
                output += white_activated[i] * network.l1_weights[i];
            }
            for (int i = 0; i < HIDDEN_SIZE; i++) {
                output += black_activated[i] * network.l1_weights[HIDDEN_SIZE + i];
            }
        } else {  // Black to move
            // STM = black, NTM = white
            for (int i = 0; i < HIDDEN_SIZE; i++) {
                output += black_activated[i] * network.l1_weights[i];
            }
            for (int i = 0; i < HIDDEN_SIZE; i++) {
                output += white_activated[i] * network.l1_weights[HIDDEN_SIZE + i];
            }
        }
        
        // Dequantize the output:
        // - screlu outputs are in range [0, QA^2] since screlu(x) = clipped^2
        // - After multiplying by l1_weights (scaled by QB), we have scale of QA^2 * QB
        // - The bias is scaled by QA * QB
        // So total scale is QA * QA * QB = QA^2 * QB
        // We need to divide by (QA * QA * QB) and multiply by SCALE
        
        int eval = (output * SCALE) / (QA * QA * QB);
        
        return eval;
    }
    
    int get_feature_index(int piece, int square, int perspective) {
        // piece: 2=KING, 3=QUEEN, 4=ROOK, 5=BISHOP, 6=KNIGHT, 7=PAWN
        // perspective: 0 = white's view, 1 = black's view
        
        // Determine piece color (0 = white, 1 = black) from the piece constant
        // White pieces: 2-7 with is_white_piece check
        // Black pieces: 2-7 with !is_white_piece check
        // We'll need to handle this at the call site since we need board context
        
        // This function assumes piece is already converted to Chess768 format (0-11)
        // and square is already oriented correctly for the perspective
        return piece * 64 + square;
    }
    
    void refresh_accumulator(Accumulator& acc, const Board& board) {
        if (!network_loaded) {
            return;
        }
        
        // Initialize with biases
        for (int i = 0; i < HIDDEN_SIZE; i++) {
            acc.white_hidden[i] = network.l0_bias[i];
            acc.black_hidden[i] = network.l0_bias[i];
        }
        
        // Accumulate features for all pieces
        for (int square = 0; square < 64; square++) {
            unsigned int piece = board.find_piece_occupying_sq(square);
            
            if (piece == PIECE_NONE || piece == PIECE_EXTRA) {
                continue;
            }
            
            int color = board.is_white_piece(square) ? 0 : 1;
            int piece_type = convert_to_chess768_piece_type(piece, color);
            if (piece_type == -1) {
                continue;
            }
            
            // White's perspective
            int white_feature = piece_type * 64 + square;
            
            // Black's perspective: flip square and swap colors
            int flipped_sq = square ^ 56;
            int flipped_piece_type = piece_type < 6 ? piece_type + 6 : piece_type - 6;
            int black_feature = flipped_piece_type * 64 + flipped_sq;
            
            // Bounds check
            if (white_feature < 0 || white_feature >= INPUT_SIZE || black_feature < 0 || black_feature >= INPUT_SIZE) {
                std::cerr << "Feature index out of bounds! white=" << white_feature << " black=" << black_feature << std::endl;
                continue;
            }
            
            // Add contributions
            for (int i = 0; i < HIDDEN_SIZE; i++) {
                acc.white_hidden[i] += network.l0_weights[white_feature * HIDDEN_SIZE + i];
                acc.black_hidden[i] += network.l0_weights[black_feature * HIDDEN_SIZE + i];
            }
        }
        
        // Invalidate cached evaluations since accumulator was refreshed
        invalidate_cache(acc);
    }
    
    void add_piece_to_accumulator(Accumulator& acc, int piece, int square, int color) {
        int piece_type = convert_to_chess768_piece_type(piece, color);
        if (piece_type == -1) {
            return;
        }
        
        // White's perspective
        int white_feature = piece_type * 64 + square;
        
        // Black's perspective
        int flipped_sq = square ^ 56;
        int flipped_piece_type = piece_type < 6 ? piece_type + 6 : piece_type - 6;
        int black_feature = flipped_piece_type * 64 + flipped_sq;
        
        // Bounds check
        if (white_feature < 0 || white_feature >= INPUT_SIZE || black_feature < 0 || black_feature >= INPUT_SIZE) {
            std::cerr << "Feature index out of bounds in add! white=" << white_feature << " black=" << black_feature 
                      << " piece=" << piece << " square=" << square << " color=" << color << std::endl;
            return;
        }
        
        // Add to both perspectives
        for (int i = 0; i < HIDDEN_SIZE; i++) {
            acc.white_hidden[i] += network.l0_weights[white_feature * HIDDEN_SIZE + i];
            acc.black_hidden[i] += network.l0_weights[black_feature * HIDDEN_SIZE + i];
        }
        
        // Invalidate cached evaluations since accumulator changed
        invalidate_cache(acc);
    }
    
    void remove_piece_from_accumulator(Accumulator& acc, int piece, int square, int color) {
        int piece_type = convert_to_chess768_piece_type(piece, color);
        if (piece_type == -1) {
            return;
        }
        
        // White's perspective
        int white_feature = piece_type * 64 + square;
        
        // Black's perspective
        int flipped_sq = square ^ 56;
        int flipped_piece_type = piece_type < 6 ? piece_type + 6 : piece_type - 6;
        int black_feature = flipped_piece_type * 64 + flipped_sq;
        
        // Bounds check
        if (white_feature < 0 || white_feature >= INPUT_SIZE || black_feature < 0 || black_feature >= INPUT_SIZE) {
            std::cerr << "Feature index out of bounds in remove! white=" << white_feature << " black=" << black_feature 
                      << " piece=" << piece << " square=" << square << " color=" << color << std::endl;
            return;
        }
        
        // Remove from both perspectives
        for (int i = 0; i < HIDDEN_SIZE; i++) {
            acc.white_hidden[i] -= network.l0_weights[white_feature * HIDDEN_SIZE + i];
            acc.black_hidden[i] -= network.l0_weights[black_feature * HIDDEN_SIZE + i];
        }
        
        // Invalidate cached evaluations since accumulator changed
        invalidate_cache(acc);
    }
    
    int evaluate_incremental(Accumulator& acc, int side_to_move) {
        if (!network_loaded) {
            std::cerr << "NNUE network not loaded!" << std::endl;
            return 0;
        }
        
        // Check if we have a cached result for this side to move
        if (side_to_move == 0 && acc.white_cache_valid) {
            return acc.cached_eval_white;
        }
        if (side_to_move == 1 && acc.black_cache_valid) {
            return acc.cached_eval_black;
        }
        
        // Apply SCReLU activation
        int32_t white_activated[HIDDEN_SIZE];
        int32_t black_activated[HIDDEN_SIZE];
        
        for (int i = 0; i < HIDDEN_SIZE; i++) {
            white_activated[i] = screlu(acc.white_hidden[i]);
            black_activated[i] = screlu(acc.black_hidden[i]);
        }
        
        // Compute output layer
        int32_t output = network.l1_bias;
        
        if (side_to_move == 0) {  // White to move
            for (int i = 0; i < HIDDEN_SIZE; i++) {
                output += white_activated[i] * network.l1_weights[i];
            }
            for (int i = 0; i < HIDDEN_SIZE; i++) {
                output += black_activated[i] * network.l1_weights[HIDDEN_SIZE + i];
            }
        } else {  // Black to move
            for (int i = 0; i < HIDDEN_SIZE; i++) {
                output += black_activated[i] * network.l1_weights[i];
            }
            for (int i = 0; i < HIDDEN_SIZE; i++) {
                output += white_activated[i] * network.l1_weights[HIDDEN_SIZE + i];
            }
        }
        
        int eval = (output * SCALE) / (QA * QA * QB);
        
        // Cache the result
        if (side_to_move == 0) {
            acc.cached_eval_white = static_cast<int16_t>(eval);
            acc.white_cache_valid = true;
        } else {
            acc.cached_eval_black = static_cast<int16_t>(eval);
            acc.black_cache_valid = true;
        }
        
        return eval;
    }
}
