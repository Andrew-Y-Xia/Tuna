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

#define PAWN_VALUE 82
#define KNIGHT_VALUE 337
#define BISHOP_VALUE 365
#define ROOK_VALUE 477
#define QUEEN_VALUE 1025

#define MOBILITY_WEIGHT 0

extern int piece_to_value[8];

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

extern U64 king_loc_queenside_castled[2];
extern U64 king_loc_kingside_castled[2];
extern U64 pawn_shield_queenside[2];
extern U64 pawn_shield_kingside[2];
extern U64 pawn_storm_queenside[2];
extern U64 pawn_storm_kingside[2];


void init_eval_utils();

void init_piece_to_value();

void init_piece_square_tables();

void index_remap(int* array);

int lookup_ps_table_m(unsigned int index, unsigned int piece, int current_turn);

int lookup_ps_table_e(unsigned int index, unsigned int piece, int current_turn);

#endif /* Evaluation_hpp */
