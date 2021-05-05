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


int cords_to_index(int x, int y) {
    return 8*(7-y) + x;
}


Cords index_to_cords(int index) {
    Cords c;
    c.x = index % 8;
    c.y = 7 - (index / 8);
    return c;
}


void print_cords(Cords c) {
    std::cout << "Cords{" << c.x << ", " << c.y << '}';
}

bool is_within_bounds(int x, int y) {
    return (0 <= x && x <= 7 && 0 <= y && y <= 7);
}

void print_move(Move move, bool reg) {
    if (!reg) {
        std::cout << "\n\n\nFrom index: " << move.get_from() << '\n';
        std::cout << "To index: " << move.get_to() << '\n';
        std::cout << "Special move flag: " << move.get_special_flag() << '\n';
        std::cout << "Promote to piece: " << move.get_promote_to() << '\n';
        std::cout << "Piece moved: " << move.get_piece_moved() << '\n';
        std::cout << "Piece captured: " << move.get_piece_captured() << '\n';
        std::cout << "Move score: " << move.get_move_score() << '\n';
    }
    else {
        Cords from_c = index_to_cords(move.get_from());
        Cords to_c = index_to_cords(move.get_to());
        std::cout << num_to_char(from_c.x) << 8 - from_c.y << num_to_char(to_c.x) << 8 - to_c.y;
        if (move.get_special_flag() == MOVE_PROMOTION) {
            switch (move.get_promote_to()) {
                case PROMOTE_TO_QUEEN:
                    std::cout << 'q';
                    break;
                case PROMOTE_TO_ROOK:
                    std::cout << 'r';
                    break;
                case PROMOTE_TO_BISHOP:
                    std::cout << 'b';
                    break;
                case PROMOTE_TO_KNIGHT:
                    std::cout << 'n';
                    break;
            }
        }
    }
}
