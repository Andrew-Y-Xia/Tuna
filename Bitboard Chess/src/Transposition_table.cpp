//
//  Transposition_table.cpp
//  Bitboard Chess
//
//  Created by Andy on 5/12/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Transposition_table.hpp"


TT::TT() {
    // Constructor, allocate the hash_table
    hash_table = new TT_entry[TT_SIZE];
    Move move;
    for (int i = 0; i < TT_SIZE; i++) {
        assert((hash_table + i)->best_move == move);
        assert((hash_table + i)->key == 0);
        assert((hash_table + i)->score == 0);
    }
}
TT::~TT() {
    // Delete hash_table
    delete[] hash_table;
}

TT_entry& TT::find(U64 key) {
    U64 lower_key = key & 0xFFFFFF;
    return *(hash_table + lower_key);
}

