//
//  UCI.cpp
//  Bitboard Chess
//
//  Created by Andy on 8/15/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "UCI.hpp"

namespace UCI {

    void init() {

    }

    void loop() {
        Thread::should_end_search = false;
        while (true) {
            std::string line;
            std::getline(std::cin, line);
            auto cmd = split(line);
            Thread::cmd_queue.enqueue(cmd);
            if (!cmd.empty()) {
                if (cmd[0] == "quit") {
                    Thread::should_end_search = true;
                    return;
                }
                else if (cmd[0] == "stop") {
                    Thread::should_end_search = true;
                }
            }
        }
    }

}
