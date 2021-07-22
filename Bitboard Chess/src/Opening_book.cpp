//
//  Opening_book.cpp
//  Bitboard Chess
//
//  Created by Andrew Xia on 7/21/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Opening_book.hpp"

void init_opening_book() {
    
    
    std::string line;
    std::ifstream myfile (resourcePath() + "opening_book.txt");
    if (myfile.is_open()) {
        while (std::getline(myfile, line)) {
            std::cout << line << '\n';
        }
    myfile.close();
    }
}
