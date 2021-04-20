//
//  Search.cpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "depend.hpp"
#include "Search.hpp"

    
Search::Search(Board& b) {
    board = b;
}

Move Search::find_best_move(int depth) {
    std::vector<Move> moves;
    moves.reserve(256);
    board.generate_moves(moves);
    board.sort_moves(moves);

    Move best_move;
    int maxEval = -2000000;

    for (auto it = moves.begin(); it != moves.end(); ++it) {
        board.process_move(*it);
        int eval = -board.negamax(depth - 1, -2000000, -maxEval);
        board.undo_last_move();

        if (eval > maxEval) {
            maxEval = eval;
            best_move = *it;
        }
        if (eval >= 2000000) {
            break;
        }
    }

    return best_move;
}


Move Search::threaded_best_move(int depth) {
    std::vector<Move> moves;
    moves.reserve(256);
    board.generate_moves(moves);
    board.sort_moves(moves);

    int i = 0;
    ctpl::thread_pool p(4);
    std::vector<std::future<int>> results(moves.size());
    for (auto it = moves.begin(); it != moves.end(); ++it) {
        board.process_move(*it);
        Board copy(board);
        results[i] = p.push(std::bind(&Board::negamax, copy, depth - 1, -2000000, 2000000));
        board.undo_last_move();
        i++;
    }

    /*
    for (i = 0; i < moves.size(); i++) {
        results[i].wait();
    }
    */


    Move best_move;
    int maxEval = -2000000;
    for (int i = 0; i < moves.size(); i++) {
        int result = -results[i].get();
        if (result > maxEval) {
            maxEval = result;
            best_move = moves[i];
        }
    }

    return best_move;
}
