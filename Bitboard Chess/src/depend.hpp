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
#include <fstream>
#include <chrono>
#include <string>
#include <ctype.h>
#include <forward_list>
#include <vector>
#include <sparsehash/sparse_hash_map>
#include <stdint.h>
#include <random>

#define WIDTH 1024
#define SCALE 1.3
#define OFFSET 2
#define ASDF std::cout << "asdf" << std::endl


#define a_file          0x0101010101010101##ULL
#define h_file          0x8080808080808080##ULL
#define first_rank      0x00000000000000FF##ULL
#define eighth_rank     0xFF00000000000000##ULL
#define a1_h8_diagonal  0x8040201008040201##ULL
#define a8_h1_diagonal  0x0102040810204080##ULL
#define light_squares   0x55AA55AA55AA55AA##ULL
#define dark_squares    0xAA55AA55AA55AA55##ULL

#define EmptyBoard    0x0000000000000000##ULL
#define UniverseBoard 0xffffffffffffffff##ULL

#define C64(constantU64) constantU64##ULL
typedef uint64_t U64;

#define bitscan_forward(a) __builtin_ctzll(a)
#define bitscan_reverse(a) (63 - __builtin_clzll(a))

#define pop_count(a) __builtin_popcountll(a)

#endif /* depend_hpp */
