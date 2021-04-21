//
//  Bitboard.cpp
//  Bitboard Chess
//
//  Created by Andy on 4/21/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Bitboard.hpp"


/**
 * generalized bitScan
 * @author Gerd Isenberg
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @param reverse, true bitScanReverse, false bitScanForward
 * @return index (0..63) of least/most significant one bit
 */
 int bitScan(U64 bb, bool reverse) {
    U64 rMask;
    assert (bb != 0);
    rMask = -(U64)reverse;
    bb &= -bb | rMask;
    return bitscan_reverse(bb);
 }

void print_BB(U64 b) {
    const U64 c = 1;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            std::cout << !!(b & (c << (8*(7-y) + x))) << ' ';
        }
        std::cout << '\n';
    }
}

void print_ls1bs(U64 x) {
    while ( x ) {
        U64 ls1b = x & -x; // isolate LS1B
        print_BB(ls1b);
        std::cout << '\n';
        x &= x-1; // reset LS1B
    }
}
