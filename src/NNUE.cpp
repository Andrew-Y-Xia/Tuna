//
//  NNUE.cpp
//  Tuna Chess Engine
//
//  NNUE (Efficiently Updatable Neural Network) Evaluation Implementation
//

#include "NNUE.hpp"
#include "Board.hpp"
#include <fstream>
#include <iostream>
#include <cstring>

namespace NNUE {
    Network network;
    bool network_loaded = false;
    
    bool init(const std::string& path) {
        return load_network(path);
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
            
            // Convert piece type to 0-based index
            int piece_type = piece - 2;  // King=0, Queen=1, ..., Pawn=5
            
            // each perspective sees their OWN pieces at their actual squares,
            // and OPPONENT pieces flipped vertically (square ^ 56)
            
            // For White's perspective:
            //   - White pieces: indices 0-383, square as-is (not flipped)
            //   - Black pieces: indices 384-767, square ^ 56 (flipped)
            // For Black's perspective:
            //   - Black pieces: indices 0-383, square as-is (not flipped)
            //   - White pieces: indices 384-767, square ^ 56 (flipped)
            
            int white_feature, black_feature;
            if (color == 0) {
                // White piece
                white_feature = piece_type * 64 + square;              // White sees white pieces normally
                black_feature = 384 + piece_type * 64 + (square ^ 56); // Black sees white pieces (opponent) flipped
            } else {
                // Black piece  
                white_feature = 384 + piece_type * 64 + (square ^ 56); // White sees black pieces (opponent) flipped
                black_feature = piece_type * 64 + square;              // Black sees black pieces normally
            }
            
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
}
