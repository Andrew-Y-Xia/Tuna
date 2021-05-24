//
//  Transposition_table.hpp
//  Bitboard Chess
//
//  Created by Andy on 5/12/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef Transposition_table_hpp
#define Transposition_table_hpp

#include "depend.hpp"
#include "Data_structs.hpp"

#define TT_SIZE 16777216 // 2^24df


#define NODE_EXACT 0
#define NODE_UPPERBOUND 1
#define NODE_LOWERBOUND 2

class HashMove: public Move {
public:
    void operator=(Move move);
    bool operator==(Move move);
    
    unsigned int get_depth() const;
    unsigned int get_node_type() const;

    void set_depth(unsigned int depth);
    void set_node_type(unsigned int node_type);
};


struct TT_entry {
    U64 key;
    HashMove hash_move;
    // No need to keep depth info because that's kept in move
    int score;
    U64 sanity_check;
};

class TT {
private:
    TT_entry* hash_table;
    unsigned int occupied;
public:
    TT();
    ~TT();
    TT_entry get(U64 key) const;
    void set(U64 key, Move best_move, unsigned int depth, unsigned int node_type, int score, U64 sanity_check);
    
};

#endif /* Transposition_table_hpp */
