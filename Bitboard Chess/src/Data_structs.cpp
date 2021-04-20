//
//  Data_structs.cpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Data_structs.hpp"


namespace Bitboard {
void print_BB(U64 b) {
    const U64 c = 1;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            std::cout << !!(b & (c << (8*(7-y) + x))) << ' ';
        }
        std::cout << '\n';
    }
}
}
