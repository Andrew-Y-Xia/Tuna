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



class OpeningBook {
private:
    std::vector<std::string> opening_lines;
    bool use_book;
public:
    OpeningBook();
    bool can_use_book();
    Move request(std::vector<Move> move_stack);
};


#endif /* Opening_book_hpp */
