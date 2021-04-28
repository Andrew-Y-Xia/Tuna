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

int cords_to_index(int x, int y);

Cords index_to_cords(int index);

void print_cords(Cords c);

bool is_within_bounds(int x, int y);

void print_move(Move move, bool reg = false);


#endif /* Utility_hpp */
