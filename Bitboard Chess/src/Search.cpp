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

    if (depth == 0) {
//            return quiescence_search(alpha, beta);
        return board.static_eval();
    }
    
    // Check for hits on the TT
    
    
    TT_entry& tt_hit = board.tt.find(board.get_z_key());
    
    if (tt_hit.key == board.get_z_key() && tt_hit.best_move.get_depth() >= depth) {
        if (tt_hit.sanity_check != board.tt_sanity_check()) {
            std::cout << "Transposition table type 1 collision\n";
        }
        
//        std::cout << tt_hit.sanity_check << ' ' << board.tt_sanity_check() << '\n';
        return tt_hit.score;
    }
    if (tt_hit.beta_flag) {
//        std::cout << tt_hit.score << '\n';
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
    
    
    // Write search data to transposition table
    tt_hit.key = board.get_z_key();
    tt_hit.best_move.set_depth(depth);
    tt_hit.score = alpha;
    tt_hit.sanity_check = board.tt_sanity_check();

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
        
        std::cout << "\nMove: ";
        print_move(it, true);
        std::cout << "\nEval: " << eval;

        if (eval > maxEval) {
            maxEval = eval;
            best_move = it;
        }
        if (eval >= 2000000) {
            break;
        }
    }
    
    
    std::cout << "\nNodes searched: " << nodes_searched << "\n\n";
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

long Search::hash_perft(unsigned int depth) {
    if (depth == 0) {
        return 1;
    }
    
    TT_entry& tt_hit = board.tt.find(board.get_z_key());
    
    if (tt_hit.key == board.get_z_key() && tt_hit.best_move.get_depth() == depth) {
        if (tt_hit.sanity_check != board.tt_sanity_check()) {
            ASDF;
        }
//        std::cout << tt_hit.sanity_check << ' ' << board.tt_sanity_check() << '\n';
        return tt_hit.score;
    }

    long nodes = 0;
    int n_moves = 0;

    MoveList moves;
    board.generate_moves(moves);
    n_moves = moves.size();


    for (auto it = moves.begin(); it != moves.end(); ++it) {
        board.make_move(*it);
        nodes += hash_perft(depth - 1);
        board.unmake_move();
    }
    
    // Write data to transposition table
    tt_hit.key = board.get_z_key();
    tt_hit.best_move.set_depth(depth);
    tt_hit.score = nodes;
    tt_hit.sanity_check = board.tt_sanity_check();
    
    return nodes;
}

