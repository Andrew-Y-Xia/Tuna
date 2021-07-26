//
//  Opening_book.cpp
//  Bitboard Chess
//
//  Created by Andrew Xia on 7/21/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Opening_book.hpp"


OpeningBook::OpeningBook() {
    use_book = true;
    
    // Loads opening book into memory
    std::string line;
    std::ifstream file (resourcePath() + "opening_book.txt");
    if (file.is_open()) {
        // 5000 is the number of lines in opening_book.txt
        // Every line is a game (PGN format)
        opening_lines.reserve(5000);
        
        while (std::getline(file, line)) {
            opening_lines.push_back(line);
        }
        file.close();
    }
    else {
        std::cout << "Opening book failed to init";
    }
}

bool OpeningBook::can_use_book() {
    return use_book;
}

Move OpeningBook::request(std::vector<Move> move_stack) {
    std::string pgn;
}
