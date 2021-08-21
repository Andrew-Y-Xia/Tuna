//
// Created by Andy on 8/18/21.
//

#ifndef BITBOARD_CHESS_TIME_HANDLER_HPP
#define BITBOARD_CHESS_TIME_HANDLER_HPP

#include "depend.hpp"
#include "Thread.hpp"

enum TimerType {
    constant_time,
    inf,
};


class TimeHandler {
private:
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<long long, std::ratio<1LL, 1000000000LL>>> start_time;
    std::atomic<bool>& should_end_search;
    TimerType timer_type;
    std::thread* t;
    double max_time_ms;

    void loop();

public:
    explicit TimeHandler(std::atomic<bool>& b, TimerType t_type = inf, double max_time_ms_input = 0);

    ~TimeHandler();

    void start();

    void stop();

    bool should_stop();
};

#endif //BITBOARD_CHESS_TIME_HANDLER_HPP
