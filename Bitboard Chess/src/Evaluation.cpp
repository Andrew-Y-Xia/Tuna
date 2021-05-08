//
//  Evaluation.cpp
//  Bitboard Chess
//
//  Created by Andrew Xia on 5/6/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Evaluation.hpp"

int piece_to_value[8];

void init_eval_utils() {
    piece_to_value[PIECE_PAWN] = PAWN_VALUE;
    piece_to_value[PIECE_KNIGHT] = KNIGHT_VALUE;
    piece_to_value[PIECE_BISHOP] = BISHOP_VALUE;
    piece_to_value[PIECE_ROOK] = ROOK_VALUE;
    piece_to_value[PIECE_QUEEN] = QUEEN_VALUE;
}
