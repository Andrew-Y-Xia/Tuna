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

        while (true) {
            std::vector<std::string> cmd = Thread::cmd_queue.dequeue();

            try {
                if (cmd.at(0) == "quit") {
                    return;
                }
                else if (cmd.at(0) == "go") {
                    if (cmd.at(1) == "perft") {
                        int perft_depth = std::stoi(cmd.at(2));

                        Search search(board, tt, opening_book);
                        auto t1 = std::chrono::high_resolution_clock::now();
                        long perft_score = search.perft(perft_depth);
                        auto t2 = std::chrono::high_resolution_clock::now();
                        std::chrono::duration<double, std::milli> ms_double = t2 - t1;
                        std::ostringstream buffer;
                        buffer << perft_score;
                        buffer << "\nTime: " << ms_double.count() << "ms\n";
                        Thread::synced_cout.print(buffer.str());
                    }
                }
            }
            catch (std::out_of_range& e) {
                std::cerr << "Insufficient parameters\n";
            }
        }
    }

}
