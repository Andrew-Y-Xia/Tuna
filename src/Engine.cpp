//
//  Engine.cpp
//  Bitboard Chess
//
//  Created by Andy on 8/15/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Engine.hpp"


namespace Engine {

Board board;
TT tt;
OpeningBook opening_book;

void init() {
    
}

void loop() {
    while (1) {
        std::string input;
        std::cin >> input;
        
        if (input == "perft") {
            Search search(board, tt, opening_book);
            
            std::cout << search.perft(5) << std::endl;
        }
    }
}

}
