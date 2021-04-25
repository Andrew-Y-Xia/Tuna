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

extern U64 rays[4][64];
extern U64 king_paths[64];
extern U64 knight_paths[64];

int bitScan(U64 bb, bool reverse);

void print_BB(U64 b);

void print_ls1bs(U64 x);

void init_bitboard_utils();

void init_king_paths();

void init_knight_paths();

#endif /* Bitboard_hpp */
