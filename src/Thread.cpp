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

#define STR_VECT std::vector<std::string>

    template SafeQueue<STR_VECT >::SafeQueue();

    template void SafeQueue<STR_VECT >::enqueue(STR_VECT t);

    template STR_VECT SafeQueue<STR_VECT >::dequeue();

    template bool SafeQueue<STR_VECT >::is_empty();

    template<class T>
    SafeQueue<T>::SafeQueue() : q(), m(), c() {};

    template<class T>
    void SafeQueue<T>::enqueue(T t) {
        // Add an element to the queue.
        std::lock_guard<std::mutex> lock(m);
        q.push(t);
        c.notify_one();
    }

    template<class T>
    T SafeQueue<T>::dequeue() {
        // Get the "front"-element.
        // If the queue is empty, wait till an element is available.
        std::unique_lock<std::mutex> lock(m);
        while (q.empty()) {
            // release lock as long as the wait and re-acquire it afterwards.
            c.wait(lock);
        }
        T val = q.front();
        q.pop();
        return val;
    }

    template<class T>
    bool SafeQueue<T>::is_empty() {
        std::lock_guard<std::mutex> lock(m);
        return q.empty();
    }

    void SyncedCout::print(const std::string& str) {
        std::unique_lock<std::mutex> guard(m);
        std::cout << str;
        std::cout.flush();
    }

    SyncedCout::SyncedCout() : m() {};

}

Thread::SyncedCout& get_synced_cout() {
    static Thread::SyncedCout synced_cout;
    return synced_cout;
}
