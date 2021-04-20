//
//  Utility.cpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Utility.hpp"
#include "Data_structs.hpp"



char num_to_char(int input) {
    char c;
    switch(input) {
        case 0:
            c = 'a';
            break;
        case 1:
            c = 'b';
            break;
        case 2:
            c = 'c';
            break;
        case 3:
            c = 'd';
            break;
        case 4:
            c = 'e';
            break;
        case 5:
            c = 'f';
            break;
        case 6:
            c = 'g';
            break;
        case 7:
            c = 'h';
            break;
    }
    return c;
}


void print_move(Move move, bool reg) {
    if (!reg) {
        std::cout << "From: " << move.from_c.x << ", " << move.from_c.y << " to: " << move.to_c.x << ", " << move.to_c.y << std::endl;
    }
    else {
        std::cout << num_to_char(move.from_c.x) << 8 - move.from_c.y << num_to_char(move.to_c.x) << 8 - move.to_c.y;
    }
}

void print_cords(Cords c) {
    std::cout << "Cords{" << c.x << ", " << c.y << '}';
}


int increment_to_index(Cords c) {
    int i = 3*(c.x + 1) + (c.y+1);
    if (i > 4) {
        i--;
    }
    return i;
}

Cords index_to_increment(int i) {
    switch (i) {
        case 0:
            return Cords{-1, -1};
        case 1:
            return Cords{-1, 0};
        case 2:
            return Cords{-1, 1};
        case 3:
            return Cords{0, -1};
        case 4:
            return Cords{0, 1};
        case 5:
            return Cords{1, -1};
        case 6:
            return Cords{1, 0};
        case 7:
            return Cords{1, 1};
        default:
            std::cout << "Should not have occured index_to_increment\n";
    }
}


void debug_print_moves(std::forward_list<Move> moves) {
    for (std::forward_list<Move>::iterator it = moves.begin() ; it != moves.end(); ++it) {
        std::cout << "From: " << it->from_c.x << ", " << it->from_c.y << "  |  To: " << it->to_c.x << ", " << it->to_c.y << "  |  Type: " << it->type << std::endl;
    }
    for (int i = 0; i < 10; i++) {
        std::cout << '\n';
    }
}
