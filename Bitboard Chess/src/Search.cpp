//
//  Search.cpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Search.hpp"

MovePicker::MovePicker(MoveList& init_moves): moves(init_moves) {
    size = init_moves.size();
    
    visit_count = 0;
}

inline int MovePicker::finished() {
    return visit_count == size;
}

inline Move MovePicker::operator++() {
    unsigned int highest_score = 0;
    int current_index = 0;
    int found_index;

    for (auto it = moves.begin(); it != moves.end(); ++it) {
        unsigned int ms = it->get_move_score();
        if (!visited[current_index] && ms > highest_score) {
            found_index = current_index;
            highest_score = ms;
        }
        current_index++;
    }
    
//    std::cout << found_index << '\n';
    visit_count++;
    visited[found_index] = true;

    return moves[found_index];
}




Search::Search(Board& b) : board(b) {}


int Search::negamax(unsigned int depth, int alpha, int beta) {

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
    
    MoveList moves;
    board.generate_moves(moves);
    
    if (moves.size() == 0) {
        if (board.is_king_in_check()) {
            return -2000000;
        }
        else {
            return 0;
        }
    }
    
//    board.sort_moves(moves);
    board.assign_move_scores(moves);
    
    
    MovePicker move_picker(moves);
    
    while (!move_picker.finished()) {
        auto it = ++move_picker;
        
        nodes_searched++;
        board.make_move(it);
        int eval = -negamax(depth - 1, -beta, -alpha);
        board.unmake_move();
        if (eval >= beta) {
            return beta;
        }
        alpha = std::max(alpha, eval);
    }

    return alpha;
}

Move Search::find_best_move(unsigned int depth) {
    nodes_searched = 0;
    
    MoveList moves;
    board.generate_moves(moves);
    board.assign_move_scores(moves);

    Move best_move;
    int maxEval = -2000001;
    
    

    
    
    MovePicker move_picker(moves);
    
    while (!move_picker.finished()) {
        auto it = ++move_picker;
        
        nodes_searched++;
        board.make_move(it);
        int eval = -negamax(depth - 1, -2000000, -maxEval);
        board.unmake_move();

        if (eval > maxEval) {
            maxEval = eval;
            best_move = it;
        }
        if (eval >= 2000000) {
            break;
        }
    }
    
    
    std::cout << "\nNodes searched: " << nodes_searched << '\n';
    return best_move;
}


long Search::perft(unsigned int depth) {
    
    if (depth == 0) {
//        print_board();
        return 1;
    }
    
    long nodes = 0;
    int n_moves = 0;

    MoveList moves;
    board.generate_moves(moves);
    n_moves = moves.size();


    for (auto it = moves.begin(); it != moves.end(); ++it) {
        board.make_move(*it);
        nodes += perft(depth - 1);
        board.unmake_move();
    }
    return nodes;
}

long Search::sort_perft(unsigned int depth) {
    
    if (depth == 0) {
//        print_board();
        return 1;
    }
    
    long nodes = 0;
    int n_moves = 0;

    MoveList moves;
    board.generate_moves(moves);
    board.assign_move_scores(moves);
    n_moves = moves.size();

    MovePicker move_picker(moves);
    while (!move_picker.finished()) {
        auto it = ++move_picker;
        
        board.make_move(it);
        nodes += sort_perft(depth-1);
        board.unmake_move();
    }
    
    return nodes;
}

