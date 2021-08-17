//
//  depend.hpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef depend_hpp
#define depend_hpp

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>
#include <ctype.h>
#include <forward_list>
#include <vector>
#include <stdint.h>
#include <stdexcept>
#include <random>

#include <assert.h>

#define ASDF std::cout << "asdf" << std::endl


#define a_file          0x0101010101010101##ULL
#define h_file          0x8080808080808080##ULL
#define first_rank      0x00000000000000FF##ULL
#define eighth_rank     0xFF00000000000000##ULL
#define a1_h8_diagonal  0x8040201008040201##ULL
#define a8_h1_diagonal  0x0102040810204080##ULL
#define light_squares   0x55AA55AA55AA55AA##ULL
#define dark_squares    0xAA55AA55AA55AA55##ULL

#define second_or_seventh_rank 0xFF00000000FF00##ULL
#define third_or_sixth_rank    0x00FF0000FF0000##ULL


#define EmptyBoard    0x0000000000000000##ULL
#define UniverseBoard 0xffffffffffffffff##ULL

#define WHITE 0
#define BLACK 1

#define C64(constantU64) constantU64##ULL
typedef uint64_t U64;

#define bitscan_forward(a) __builtin_ctzll(a)
#define bitscan_reverse(a) (63 - __builtin_clzll(a))

#define pop_count(a) __builtin_popcountll(a)

#define byteswap(a) __builtin_bswap64(a)

std::string resource_path();

#endif /* depend_hpp */
