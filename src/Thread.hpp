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

    extern std::atomic<bool> should_end_search;


    template<class T>
    class SafeQueue {
    private:
        std::queue<T> q;
        mutable std::mutex m;
        std::condition_variable c;
    public:
        SafeQueue() : q(), m(), c() {};

        // Add an element to the queue.
        void enqueue(T t) {
            std::lock_guard<std::mutex> lock(m);
            q.push(t);
            c.notify_one();
        }

        // Get the "front"-element.
        // If the queue is empty, wait till an element is available.
        T dequeue() {
            std::unique_lock<std::mutex> lock(m);
            while (q.empty()) {
                // release lock as long as the wait and re-acquire it afterwards.
                c.wait(lock);
            }
            T val = q.front();
            q.pop();
            return val;
        }
    };

    extern SafeQueue<std::vector<std::string>> cmd_queue;

    class SyncedCout {
    private:
        std::mutex m;
    public:
        void print(const std::string& str);
    };

    extern SyncedCout synced_cout;
}

#endif /* Thread_hpp */
