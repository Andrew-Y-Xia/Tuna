//
//  Utility.hpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef Utility_hpp
#define Utility_hpp

#include "depend.hpp"
#include "Data_structs.hpp"

// Math function: sign
template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

unsigned int flip_index_v(unsigned int i);


char num_to_char(int input);

int char_to_num(char input);

unsigned int piece_char_to_piece(char input);

int cords_to_index(int x, int y);

Cords index_to_cords(int index);

void print_cords(Cords c);

bool is_within_bounds(int x, int y);

void print_move(Move move, bool reg = false);

unsigned int txt_square_to_index(std::string str);


namespace converter {

old::piece_type piece_type_to_old(unsigned int piece);
old::Move move_to_old(Move move);

unsigned int old_piece_type_to_new(old::piece_type piece);
Move old_move_to_new(old::Move old_move);

}


#endif /* Utility_hpp */
