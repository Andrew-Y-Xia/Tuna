//
//  Transposition_table.cpp
//  Bitboard Chess
//
//  Created by Andy on 5/12/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Transposition_table.hpp"


U64 constexpr TT_SIZE() {
    return C64(1) << TT_EXP_2_SIZE;
}

U64 constexpr TT_LOOKUP_MASK() {
    return TT_SIZE() - 1;
}

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

Move HashMove::to_move() {
    return Move(move_data);
}

unsigned int upper_bits_to_u32(U64 input) {
    return (input >> 32);
}

TT::TT() {
    // Constructor, allocate the hash_table
    hash_table = new bucket[TT_SIZE()];
    Move move;
    for (int i = 0; i < TT_SIZE(); i++) {
        assert((hash_table + i)->entries[0].hash_move == move);
        assert((hash_table + i)->entries[0].key == 0);
        assert((hash_table + i)->entries[0].score == 0);
    }
}

TT::~TT() {
    // Delete hash_table
    delete[] hash_table;
}

TT_result TT::get(U64 key) const {
    U64 lower_key = key & TT_LOOKUP_MASK();
    unsigned int upper_key = upper_bits_to_u32(key);
    bucket b = *(hash_table + lower_key);
    for (int i = 0; i < BUCKET_SIZE; i++) {
        TT_entry tt_entry = b.entries[i];
        if (tt_entry.key == upper_key) {
            return TT_result{tt_entry, true};
        }
    }
    return TT_result{TT_entry(), false};
}

void TT::prefetch(U64 key) const {
    U64 lower_key = key & TT_LOOKUP_MASK();
    __builtin_prefetch(hash_table + lower_key, 1);
}

void set_tt_entry(TT_entry& entry, unsigned int upper_key, Move best_move, unsigned int depth, unsigned int node_type, int score) {
    entry.key = upper_key;
    entry.hash_move = best_move;
    entry.hash_move.set_depth(depth);
    entry.hash_move.set_node_type(node_type);
    entry.score = score;
    entry.age = 0;
}

void TT::set(U64 key, Move best_move, unsigned int depth, unsigned int node_type, int score) {
    U64 lower_key = key & TT_LOOKUP_MASK();
    unsigned int upper_key = upper_bits_to_u32(key);
    bucket* b = hash_table + lower_key;

    // Pass one
    // Replace empty entries or entries with matching key
    for (int i = 0; i < BUCKET_SIZE; i++) {
        TT_entry& entry = b->entries[i];
        if (entry.key == upper_key || entry.hash_move.get_raw_data() == 0) {
            set_tt_entry(entry, upper_key, best_move, depth, node_type, score);
            return;
        }
    }

    // Pass two
    // Replace entry from older search
    for (int i = 0; i < BUCKET_SIZE; i++) {
        TT_entry& entry = b->entries[i];
        if (entry.age > 0 && entry.hash_move.get_node_type() != NODE_EXACT) {
            set_tt_entry(entry, upper_key, best_move, depth, node_type, score);
            return;
        }
    }

    // Pass three
    // Replace the lowest depth entry
    unsigned int min_depth = 20000;
    int min_index = -1;
    for (int i = 0; i < BUCKET_SIZE; i++) {
        TT_entry& entry = b->entries[i];
        unsigned int entry_depth = entry.hash_move.get_depth();
        if (entry_depth < min_depth && entry.hash_move.get_node_type() != NODE_EXACT) {
            min_depth = entry_depth;
            min_index = i;
        }
    }
    if (min_index != -1) {
        set_tt_entry(b->entries[min_index], upper_key, best_move, depth, node_type, score);
        return;
    }

    // If none of the entries were replaceable it means they're all PV nodes
    for (int i = 0; i < BUCKET_SIZE; i++) {
        assert(b->entries[0].hash_move.get_node_type() == NODE_EXACT);
        assert(b->entries[1].hash_move.get_node_type() == NODE_EXACT);
        assert(b->entries[2].hash_move.get_node_type() == NODE_EXACT);
        assert(b->entries[3].hash_move.get_node_type() == NODE_EXACT);
    }

    // PV node specific pass
    // PV node must be replaced
    if (node_type == NODE_EXACT) {
        for (int i = 0; i < BUCKET_SIZE; i++) {
            TT_entry& entry = b->entries[i];
            if (entry.hash_move.get_node_type() != NODE_EXACT) {
                set_tt_entry(entry, upper_key, best_move, depth, node_type, score);
                return;
            } else if (entry.age >= 0) {
                set_tt_entry(entry, upper_key, best_move, depth, node_type, score);
                return;
            }
        }
        abort();
    }
}


void TT::clear() {
    memset(hash_table, 0, TT_SIZE() * sizeof(*hash_table));
    Move move;
    for (int i = 0; i < TT_SIZE(); i++) {
        assert((hash_table + i)->entries[0].hash_move == move);
        assert((hash_table + i)->entries[0].key == 0);
        assert((hash_table + i)->entries[0].score == 0);
    }
}

