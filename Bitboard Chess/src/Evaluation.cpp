//
//  Evaluation.cpp
//  Bitboard Chess
//
//  Created by Andrew Xia on 5/6/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Evaluation.hpp"

int piece_to_value[8];
int piece_to_value_small[8];

void init_eval_utils() {
    piece_to_value[PIECE_PAWN] = PAWN_VALUE;
    piece_to_value[PIECE_KNIGHT] = KNIGHT_VALUE;
    piece_to_value[PIECE_BISHOP] = BISHOP_VALUE;
    piece_to_value[PIECE_ROOK] = ROOK_VALUE;
    piece_to_value[PIECE_QUEEN] = QUEEN_VALUE;
    
    piece_to_value_small[PIECE_PAWN] = PAWN_VALUE / 10;
    piece_to_value_small[PIECE_KNIGHT] = KNIGHT_VALUE / 10;
    piece_to_value_small[PIECE_BISHOP] = BISHOP_VALUE / 10;
    piece_to_value_small[PIECE_ROOK] = ROOK_VALUE / 10;
    piece_to_value_small[PIECE_QUEEN] = QUEEN_VALUE / 10;
}
