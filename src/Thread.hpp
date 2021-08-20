//
//  Thread.hpp
//  Bitboard Chess
//
//  Created by Andy on 8/15/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef Thread_hpp
#define Thread_hpp

#include "depend.hpp"

#include <thread>
#include <atomic>
#include <condition_variable>
#include <queue>

namespace Thread {


    template<class T>
    class SafeQueue {
    private:
        std::queue<T> q;
        mutable std::mutex m;
        std::condition_variable c;
    public:
        SafeQueue();

        void enqueue(T t);

        T dequeue();

        bool is_empty();
    };

    class SyncedCout {
    private:
        std::mutex m;
    public:
        SyncedCout();
        void print(const std::string& str);
    };
}

Thread::SyncedCout& get_synced_cout();

#endif /* Thread_hpp */
