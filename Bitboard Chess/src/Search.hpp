//
//  Search.hpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef Search_hpp
#define Search_hpp

#include "Board.hpp"
#include "ctpl_stl.h"


class MovePicker {
private:
    MoveList& moves;
    bool visited[256] = {0};
    int size, visit_count;
public:
    MovePicker(MoveList& init_moves);
    
    int finished();
    Move operator++();
};


struct SearchTimeout : public std::exception {};


class Search {
private:
    Board& board;
    unsigned int nodes_searched;
    std::chrono::time_point<std::__1::chrono::steady_clock, std::chrono::duration<long long, std::ratio<1LL, 1000000000LL>>> start_time;
    double max_time_ms;
public:
    
    Search(Board& b);
    
    int negamax(unsigned int depth, int alpha, int beta, unsigned int ply_from_root);
    Move find_best_move(unsigned int max_depth, double max_time_ms);

    long perft(unsigned int depth);
    long sort_perft(unsigned int depth);
    
    long hash_perft(unsigned int depth);
    
};

#endif /* Search_hpp */
