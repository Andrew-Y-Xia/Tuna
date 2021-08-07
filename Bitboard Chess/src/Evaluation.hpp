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
#include "Utility.hpp"

#define PAWN_VALUE 100
#define KNIGHT_VALUE 400
#define BISHOP_VALUE 400
#define ROOK_VALUE 600
#define QUEEN_VALUE 1200

#define MOBILITY_WEIGHT 2

extern int piece_to_value[8];
extern int piece_to_value_small[8];

extern int pawn_ps_m[64];
extern int pawn_ps_e[64];
extern int knight_ps_m[64];
extern int knight_ps_e[64];
extern int bishop_ps_m[64];
extern int bishop_ps_e[64];
extern int rook_ps_m[64];
extern int rook_ps_e[64];
extern int queen_ps_m[64];
extern int queen_ps_e[64];
extern int king_ps_m[64];
extern int king_ps_e[64];

extern int* ps_m[6];
extern int* ps_e[6];


void init_eval_utils();
void init_piece_to_value();
void init_piece_square_tables();

void index_remap(int* array);

int lookup_ps_table_m(unsigned int index, unsigned int piece, int current_turn);
int lookup_ps_table_e(unsigned int index, unsigned int piece, int current_turn);

#endif /* Evaluation_hpp */
