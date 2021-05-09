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

class Search {
private:
    Board board;
    unsigned int nodes_searched;
public:
    
    Search(Board& b);
    
    int negamax(int depth, int alpha, int beta);
    Move find_best_move(int depth);
    
};

#endif /* Search_hpp */
