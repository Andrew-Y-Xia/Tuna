//
// Created by Andy on 8/18/21.
//

#include "Time_handler.hpp"

TimeHandler::TimeHandler(std::atomic<bool>& b, TimerType t_type, double max_time_ms_input) : should_end_search(b),
                                                                                             timer_type(t_type),
                                                                                             max_time_ms(
                                                                                                     max_time_ms_input) {};

TimeHandler::~TimeHandler() {
    if (t) {
        if (t->joinable()) {
            t->join();
        }
        delete t;
        t = nullptr;
    }
}

void TimeHandler::start() {
    should_end_search = true;
    if (t) {
        if (t->joinable()) {
            t->join();
        }
        delete t;
    }
    should_end_search = false;
    start_time = std::chrono::steady_clock::now();
    t = new std::thread(&TimeHandler::loop, this);
}

void TimeHandler::loop() {
    if (timer_type == constant_time) {
        while (true) {
            std::chrono::duration<double, std::milli> ms_double = std::chrono::steady_clock::now() - start_time;
            if (ms_double.count() >= max_time_ms || should_end_search) {
                should_end_search = true;
                return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }
    } else if (timer_type == inf) {
        return;
    }
}

bool TimeHandler::should_stop() {
    bool b = should_end_search;
    return b;
}

void TimeHandler::stop() {
    should_end_search = true;
    assert(t);
    if (t->joinable()) {
        t->join();
    }
    delete t;
    t = nullptr;
}
