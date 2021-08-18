//
//  Engine.hpp
//  Bitboard Chess
//
//  Created by Andy on 8/15/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef Engine_hpp
#define Engine_hpp

#include "depend.hpp"
#include "Board.hpp"
#include "Search.hpp"
#include "Transposition_table.hpp"
#include "Opening_book.hpp"
#include "Thread.hpp"


class Engine {
private:
    Thread::SyncedCout& synced_cout;
    Thread::SafeQueue<std::vector<std::string>>& cmd_queue;
    std::atomic<bool>& should_end_search;
public:
    Engine(Thread::SyncedCout& s, Thread::SafeQueue<std::vector<std::string>>& c, std::atomic<bool>& b);
    void loop();
    std::thread spawn();
};

#endif /* Engine_hpp */
