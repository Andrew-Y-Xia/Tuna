//
//  Engine.hpp
//  Bitboard Chess
//
//  Created by Andy on 8/15/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef Engine_hpp
#define Engine_hpp

#include "depend.hpp"
#include "Board.hpp"
#include "Search.hpp"
#include "Transposition_table.hpp"
#include "Opening_book.hpp"
#include "Thread.hpp"


namespace Engine {


void init();
void loop();


}

#endif /* Engine_hpp */
