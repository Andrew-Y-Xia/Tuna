//
//  UCI.cpp
//  Bitboard Chess
//
//  Created by Andy on 8/15/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "UCI.hpp"

UCI::UCI(Thread::SafeQueue<std::vector<std::string>>& c, std::atomic<bool>& b) : cmd_queue(c), should_end_search(b) {};

void init_uci() {
    while (true) {
        std::string line;
        std::getline(std::cin, line);
        auto cmd = split(line);
        if (cmd[0] == "isready") {
            return;
        } else if (cmd[0] == "uci") {
            get_synced_cout().print("id name Bitboard_Chess\n");
            get_synced_cout().print("id author Andrew_Xia\n");
            get_synced_cout().print("uciok\n");
        }
    }
}

void UCI::loop() {
    get_synced_cout().print("readyok\n");
    while (true) {
        std::string line;
        std::getline(std::cin, line);
        auto cmd = split(line);
        cmd_queue.enqueue(cmd);
        if (!cmd.empty()) {
            if (cmd[0] == "quit") {
                should_end_search = true;
                return;
            } else if (cmd[0] == "stop") {
                should_end_search = true;
            } else if (cmd[0] == "isready") {
                while (!cmd_queue.is_empty()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(5));
                }
                get_synced_cout().print("readyok\n");
            }
        }
    }
}

