//
//  Transposition_table.cpp
//  Bitboard Chess
//
//  Created by Andy on 5/12/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Transposition_table.hpp"

void HashMove::operator=(Move move) {
    move_data = move.get_raw_data() & 0x3FFFFF;
}

bool HashMove::operator==(Move move) {
    return (move_data & 0x3FFFFF) == (move.get_raw_data() & 0x3FFFFF);
}

unsigned int HashMove::get_depth() const {
    return (move_data >> 22) & 0x3F;
}
unsigned int HashMove::get_node_type() const {
    return (move_data >> 28) & 0xF;
}

void HashMove::set_depth(unsigned int depth) {
    move_data &= ~(0x3F << 22);
    move_data |= (depth & 0x3F) << 22;
}
void HashMove::set_node_type(unsigned int node_type) {
    move_data &= ~(0xF << 28);
    move_data |= (node_type & 0xF) << 28;
}



TT::TT() {
    // Constructor, allocate the hash_table
    hash_table = new TT_entry[TT_SIZE];
    Move move;
    for (int i = 0; i < TT_SIZE; i++) {
        assert((hash_table + i)->hash_move == move);
        assert((hash_table + i)->key == 0);
        assert((hash_table + i)->score == 0);
    }
}
TT::~TT() {
    // Delete hash_table
    delete[] hash_table;
}

TT_entry TT::get(U64 key) const {
    U64 lower_key = key & C64(0xFFFFFF);
    return *(hash_table + lower_key);
}

void TT::set(U64 key, Move best_move, unsigned int depth, unsigned int node_type, int score) {
    U64 lower_key = key & C64(0xFFFFFF);
    (hash_table + lower_key)->key = key;
    (hash_table + lower_key)->hash_move = best_move;
    (hash_table + lower_key)->hash_move.set_depth(depth);
    (hash_table + lower_key)->hash_move.set_node_type(node_type);
    (hash_table + lower_key)->score = score;
}

