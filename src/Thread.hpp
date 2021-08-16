//
//  Thread.hpp
//  Bitboard Chess
//
//  Created by Andy on 8/15/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef Thread_hpp
#define Thread_hpp

#include <thread>
#include <atomic>

namespace Thread {

    extern std::atomic<bool> should_end_search;


}

#endif /* Thread_hpp */
