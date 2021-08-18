//
// Created by Andy on 8/18/21.
//

#ifndef BITBOARD_CHESS_TIME_HANDLER_HPP
#define BITBOARD_CHESS_TIME_HANDLER_HPP

#include "depend.hpp"
#include "Thread.hpp"


class TimeHandler {
private:
    std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<long long, std::ratio<1LL, 1000000000LL>>> start_time;
    std::atomic<bool>& should_end_search;
    std::thread* t;
    double max_time_ms;

    void loop();
public:
    explicit TimeHandler(std::atomic<bool>& b, double max_time_ms_input);
    ~TimeHandler();

    void start();

    void stop();

    bool should_stop();
};

#endif //BITBOARD_CHESS_TIME_HANDLER_HPP
