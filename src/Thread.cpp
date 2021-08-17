//
//  Thread.cpp
//  Bitboard Chess
//
//  Created by Andy on 8/15/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Thread.hpp"

namespace Thread {

    std::atomic<bool> should_end_search;
    SafeQueue<std::vector<std::string>> cmd_queue;

    void SyncedCout::print(const std::string& str) {
        std::lock_guard<std::mutex> guard(m);
        std::cout << str;
    }

    SyncedCout synced_cout;

}
