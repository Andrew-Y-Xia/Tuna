//
//  Bitboard.hpp
//  Bitboard Chess
//
//  Created by Andy on 4/21/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef Bitboard_hpp
#define Bitboard_hpp

#include "depend.hpp"
#include "Data_structs.hpp"
#include "Utility.hpp"

extern U64 rays[8][64];
extern U64 king_paths[64];
extern U64 knight_paths[64];
extern U64 pawn_attacks[2][64];
extern Directions direction_between[64][64];

extern U64 rook_rays[64];
extern U64 bishop_rays[64];

U64 eastOne(U64 b);

U64 noEaOne(U64 b);

U64 soEaOne(U64 b);

U64 westOne(U64 b);

U64 soWeOne(U64 b);

U64 noWeOne(U64 b);

int bitScan(U64 bb, bool reverse);

void print_BB(U64 b);

void print_ls1bs(U64 x);

void init_bitboard_utils();

void init_king_paths();

void init_knight_paths();

void init_rays();

void init_pawn_attacks();

void init_direction_between();

#endif /* Bitboard_hpp */
