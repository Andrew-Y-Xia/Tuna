//
//  depend.hpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef depend_hpp
#define depend_hpp

#include <SFML/Graphics.hpp>
#include <iostream>
#include <chrono>
#include <string>
#include <ctype.h>
#include <forward_list>
#include <vector>
#include <sparsehash/dense_hash_map>
#include <stdint.h>
#include <nmmintrin.h>
#include <immintrin.h>

#define WIDTH 1024
#define SCALE 1.3
#define OFFSET 2
#define ASDF std::cout << "asdf" << std::endl

#define PAWN_VALUE 10
#define KNIGHT_VALUE 30
#define BISHOP_VALUE 33
#define ROOK_VALUE 50
#define QUEEN_VALUE 90

#define a_file          0x0101010101010101
#define h_file          0x8080808080808080
#define first_rank      0x00000000000000FF
#define eighth_rank     0xFF00000000000000
#define a1_h8_diagonal  0x8040201008040201
#define a8_h1_diagonal  0x0102040810204080
#define light_squares   0x55AA55AA55AA55AA
#define dark_squares    0xAA55AA55AA55AA55

#define EmptyBoard    0x0000000000000000
#define UniverseBoard 0xffffffffffffffff

#define C64(constantU64) constantU64##ULL
typedef uint64_t U64;

#define bitscan_forward(a) _tzcnt_u64(a)
#define bitscan_reverse(a) (63 - _lzcnt_u64(a))

#endif /* depend_hpp */
