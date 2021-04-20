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


char num_to_char(int input);

void print_move(Move move, bool reg = false);

void print_cords(Cords c);

int increment_to_index(Cords c);

Cords index_to_increment(int i);

void debug_print_moves(std::forward_list<Move> moves);

#endif /* Utility_hpp */
