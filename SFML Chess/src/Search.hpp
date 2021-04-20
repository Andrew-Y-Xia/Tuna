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
public:
    
    Search(Board& b);
    
    Move find_best_move(int depth);
    
    
    Move threaded_best_move(int depth);
    
};

#endif /* Search_hpp */
