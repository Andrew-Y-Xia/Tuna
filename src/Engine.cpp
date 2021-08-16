//
//  Engine.cpp
//  Bitboard Chess
//
//  Created by Andy on 8/15/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Engine.hpp"


namespace Engine {


    void loop() {
        Board board;
        TT tt;
        OpeningBook opening_book;

        while (1) {
            std::string input;
            std::cin >> input;

            if (input == "perft") {
                Search search(board, tt, opening_book);
                auto t1 = std::chrono::high_resolution_clock::now();
                long perft_score = search.perft(6);
                auto t2 = std::chrono::high_resolution_clock::now();
                std::chrono::duration<double, std::milli> ms_double = t2 - t1;
                std::cout << perft_score;

                std::cout << "\nTime: " << ms_double.count() << "ms\n";
            }
        }
    }
    
}
