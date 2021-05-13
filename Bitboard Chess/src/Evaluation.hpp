//
//  Evaluation.hpp
//  Bitboard Chess
//
//  Created by Andrew Xia on 5/6/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef Evaluation_hpp
#define Evaluation_hpp

#include "depend.hpp"
#include "Data_structs.hpp"

#define PAWN_VALUE 100
#define KNIGHT_VALUE 300
#define BISHOP_VALUE 300
#define ROOK_VALUE 500
#define QUEEN_VALUE 900

extern int piece_to_value[8];
extern int piece_to_value_small[8];


void init_eval_utils();
void init_piece_to_value();
void init_piece_square_tables();

#endif /* Evaluation_hpp */
