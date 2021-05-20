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


struct TT_entry {
    U64 key;
    Move best_move;
    // No need to keep depth info because that's kept in move
    int score;
    U64 sanity_check;
    int beta_flag;
};

class TT {
private:
    TT_entry* hash_table;
    unsigned int occupied;
public:
    TT();
    ~TT();
    TT_entry& find(U64 key);
    
};

#endif /* Transposition_table_hpp */
