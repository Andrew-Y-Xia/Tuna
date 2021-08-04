//
//  Opening_book.hpp
//  Bitboard Chess
//
//  Created by Andrew Xia on 7/21/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef Opening_book_hpp
#define Opening_book_hpp

#include "depend.hpp"
#include "ResourcePath.hpp"
#include "Data_structs.hpp"
#include "Board.hpp"

#include <sstream>
#include <algorithm>
#include <iterator>



class OpeningBook {
private:
    std::vector<std::vector<Move>> opening_lines;
    bool use_book;
public:
    OpeningBook();
    bool can_use_book();
    void set_use_book(bool b);
    Move request(std::vector<move_data> move_stack);
    void reset();
};


#endif /* Opening_book_hpp */
