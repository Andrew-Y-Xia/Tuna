//
//  Ray_gen.hpp
//  Bitboard Chess
//
//  Created by Andrew Xia on 8/6/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef Ray_gen_hpp
#define Ray_gen_hpp

#include "depend.hpp"
#include "Bitboard.hpp"
#include "Data_structs.hpp"

extern const U64 RMagic[64];

extern const U64 BMagic[64];

extern U64 bishop_move_table[64][1024];
extern U64 rook_move_table[64][4096];


U64 get_positive_ray_attacks(int from_square, Directions dir, U64 occ);
U64 get_negative_ray_attacks(int from_square, Directions dir, U64 occ);

U64 bishop_attacks_classical(int from_index, U64 occ);
U64 rook_attacks_classical(int from_index, U64 occ);
U64 bishop_attacks(int from_index, U64 occ);
U64 rook_attacks(int from_index, U64 occ);
U64 xray_bishop_attacks(int from_index, U64 occ, U64 blockers);
U64 xray_rook_attacks(int from_index, U64 occ, U64 blockers);

U64 in_between_mask(int from_index, int to_index);

U64 gen_blockerboard (int index, U64 blockermask);

void init_ray_gen();


#endif /* Ray_gen_hpp */
