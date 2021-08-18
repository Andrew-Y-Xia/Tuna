//
//  Engine.cpp
//  Bitboard Chess
//
//  Created by Andy on 8/15/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Engine.hpp"



Engine::Engine(Thread::SyncedCout& s, Thread::SafeQueue<std::vector<std::string>>& c, std::atomic<bool>& b) : synced_cout(s), cmd_queue(c), should_end_search(b) {};

void Engine::loop() {
    Board board;
    TT tt;
    OpeningBook opening_book;
    TimeHandler th_blank(should_end_search, 0);

    while (true) {
        std::vector<std::string> cmd = cmd_queue.dequeue();

        try {
            if (cmd.at(0) == "quit") {
                return;
            }
            else if (cmd.at(0) == "go") {
                if (cmd.at(1) == "perft") {
                    int perft_depth = std::stoi(cmd.at(2));

                    Search search(board, tt, opening_book, th_blank);
                    auto t1 = std::chrono::high_resolution_clock::now();
                    long perft_score = search.perft(perft_depth);
                    auto t2 = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double, std::milli> ms_double = t2 - t1;
                    std::ostringstream buffer;
                    buffer << perft_score;
                    buffer << "\nTime: " << ms_double.count() << "ms\n";
                    synced_cout.print(buffer.str());
                }
                else if (cmd.at(1) == "search") {
                    int time_ms = std::stoi(cmd.at(2));
                    TimeHandler time_handler(should_end_search, time_ms);
                    Search search(board, tt, opening_book, time_handler);
                    Move move = search.find_best_move(64);
                    synced_cout.print(move_to_str(move, true) + "\n");
                }
            }
        }
        catch (std::out_of_range& e) {
            std::cerr << "Insufficient parameters\n";
        }
    }
}

std::thread Engine::spawn() {
    return std::thread(&Engine::loop, this);
}

