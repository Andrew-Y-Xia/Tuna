//
//  Transposition_table.hpp
//  Bitboard Chess
//
//  Created by Andy on 5/12/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef Transposition_table_hpp
#define Transposition_table_hpp

#include <algorithm>

#include "depend.hpp"
#include "Data_structs.hpp"

#define TT_EXP_2_SIZE 22 // TT_SIZE is 2^x
#define BUCKET_SIZE 4

#define NODE_EXACT 0
#define NODE_UPPERBOUND 1
#define NODE_LOWERBOUND 2


class HashMove : public Move {
public:
    void operator=(Move move);

    bool operator==(Move move);

    unsigned int get_depth() const;

    unsigned int get_node_type() const;

    void set_depth(unsigned int depth);

    void set_node_type(unsigned int node_type);

    Move to_move();
};


struct TT_entry {
    unsigned int key;
    HashMove hash_move;
    // No need to keep depth info because that's kept in move
    int score;
    unsigned int age;
};

struct bucket {
    TT_entry entries[BUCKET_SIZE];
};

struct TT_result {
    TT_entry tt_entry;
    bool is_hit;
};

class TT {
private:
    bucket* hash_table;
public:
    TT();

    ~TT();

    TT_result get(U64 key) const;

    void prefetch(U64 key) const;

    void set(U64 key, Move best_move, unsigned int depth, unsigned int node_type, int score);

    void increment_age();

    void clear();

};


#endif /* Transposition_table_hpp */
