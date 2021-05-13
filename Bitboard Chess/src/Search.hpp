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
    std::vector<Move>& moves;
    bool visited[256] = {0};
    int size, visit_count;
public:
    MovePicker(std::vector<Move>& init_moves);
    
    int finished();
    Move operator++();
};

class Search {
private:
    Board& board;
    unsigned int nodes_searched;
public:
    
    Search(Board& b);
    
    int negamax(unsigned int depth, int alpha, int beta);
    Move find_best_move(unsigned int depth);
    
    long perft(unsigned int depth);
    long sort_perft(unsigned int depth);
    
};

#endif /* Search_hpp */
