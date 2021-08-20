//
//  UCI.hpp
//  Bitboard Chess
//
//  Created by Andy on 8/15/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef UCI_hpp
#define UCI_hpp

#include "depend.hpp"
#include "Thread.hpp"
#include "Utility.hpp"
#include "Board.hpp"

void init_uci();

class UCI {
private:
    Thread::SafeQueue<std::vector<std::string>>& cmd_queue;
    std::atomic<bool>& should_end_search;

public:
    UCI(Thread::SafeQueue<std::vector<std::string>>& c, std::atomic<bool>& b);

    void loop();

};

#endif /* UCI_hpp */
