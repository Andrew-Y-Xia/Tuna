//
//  Zobrist.hpp
//  Bitboard Chess
//
//  Created by Andrew Xia on 8/14/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef Zobrist_hpp
#define Zobrist_hpp

#include "depend.hpp"

extern U64 piece_bitstrings[64][2][6];
extern U64 black_to_move_bitstring;
extern U64 white_castle_queenside_bitstring, white_castle_kingside_bitstring, black_castle_queenside_bitstring, black_castle_kingside_bitstring;
extern U64 en_passant_bitstrings[8];

void init_zobrist_bitstrings();

#endif /* Zobrist_hpp */
