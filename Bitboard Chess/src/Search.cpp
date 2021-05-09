//
//  Search.cpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Search.hpp"

Search::Search(Board& b) {
    board = b;
}

int Search::negamax(int depth, int alpha, int beta) {

    /*
    if (has_been_checkmated(moves)) {
        return get_current_turn() == 0 ? 2000000 : -2000000;
    }
    else if (is_draw(moves)) {
        return 0;
    }
     */
    if (depth == 0) {
//            return quiescence_search(alpha, beta);
        return board.static_eval();
    }
    
    std::vector<Move> moves;
    moves.reserve(256);
    board.generate_moves(moves);
    
    if (moves.size() == 0) {
        if (board.is_king_in_check()) {
            return board.get_current_turn() ? -2000000 : 2000000;
        }
        else {
            return 0;
        }
    }
    
    board.sort_moves(moves);



    for (auto it = moves.begin(); it != moves.end(); ++it) {
        nodes_searched++;
        board.make_move(*it);
        int eval = -negamax(depth - 1, -beta, -alpha);
        board.unmake_move();
        if (eval >= beta) {
            return beta;
        }
        alpha = std::max(alpha, eval);
    }

    return alpha;
}

Move Search::find_best_move(int depth) {
    nodes_searched = 0;
    
    std::vector<Move> moves;
    moves.reserve(256);
    board.generate_moves(moves);
    board.sort_moves(moves);

    Move best_move;
    int maxEval = -2000000;

    for (auto it = moves.begin(); it != moves.end(); ++it) {
        nodes_searched++;
        board.make_move(*it);
        int eval = -negamax(depth - 1, -2000000, -maxEval);
        board.unmake_move();

        if (eval > maxEval) {
            maxEval = eval;
            best_move = *it;
        }
        if (eval >= 2000000) {
            break;
        }
    }
    
    std::cout << "\nNodes searched: " << nodes_searched << '\n';
    return best_move;
}
