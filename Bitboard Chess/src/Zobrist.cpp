//
//  Zobrist.cpp
//  Bitboard Chess
//
//  Created by Andrew Xia on 8/14/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Zobrist.hpp"

U64 piece_bitstrings[64][2][6];
U64 black_to_move_bitstring;
U64 white_castle_queenside_bitstring, white_castle_kingside_bitstring, black_castle_queenside_bitstring, black_castle_kingside_bitstring;
U64 en_passant_bitstrings[8];

void init_zobrist_bitstrings() {
    // Initializes bitstrings used for zobrist hashing
    
//    std::random_device rd;
    
    // Random number generator
    std::default_random_engine generator(42);

    // Distribution on which to apply the generator (uniform, from 0 to 2^64 - 1)
    std::uniform_int_distribution<U64> distribution(0,0xFFFFFFFFFFFFFFFF);

    for (int i = 0; i < 64; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 6; k++) {
                piece_bitstrings[i][j][k] = distribution(generator);
            }
        }
    }
    black_to_move_bitstring = distribution(generator);
    
    white_castle_queenside_bitstring = distribution(generator);
    white_castle_kingside_bitstring = distribution(generator);
    black_castle_queenside_bitstring = distribution(generator);
    black_castle_kingside_bitstring = distribution(generator);
    
    for (int i = 0; i < 8; i++) {
        en_passant_bitstrings[i] = distribution(generator);
    }
}
