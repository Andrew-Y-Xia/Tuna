//
//  UCI.cpp
//  Bitboard Chess
//
//  Created by Andy on 8/15/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "UCI.hpp"

UCI::UCI(Thread::SyncedCout& s, Thread::SafeQueue<std::vector<std::string>>& c, std::atomic<bool>& b) : synced_cout(s), cmd_queue(c), should_end_search(b) {};

void UCI::loop() {
    synced_cout.print("Start\n");
    while (true) {
        std::string line;
        std::getline(std::cin, line);
        auto cmd = split(line);
        cmd_queue.enqueue(cmd);
        if (!cmd.empty()) {
            if (cmd[0] == "quit") {
                should_end_search = true;
                return;
            }
            else if (cmd[0] == "stop") {
                should_end_search = true;
            }
        }
    }
}
