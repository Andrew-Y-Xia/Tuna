//
//  Search.cpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Search.hpp"


int type2collision;

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
    // Check for hits on the TT
    const TT_entry tt_hit = board.tt.get(board.get_z_key());
    
    if (tt_hit.key == board.get_z_key() && tt_hit.hash_move.get_depth() >= depth) {
        if (tt_hit.sanity_check != board.tt_sanity_check()) {
            std::cout << "Transposition table type 1 collision\n";
        }
        
        int score = tt_hit.score;
        unsigned int node_type = tt_hit.hash_move.get_node_type();
        if (node_type == NODE_EXACT) {
            return score;
        }
        if (node_type == NODE_UPPERBOUND && score <= alpha) {
            return score;
        }
        if (node_type == NODE_LOWERBOUND && score >= beta) {
            return score;
        }
    }
    
    if (tt_hit.key != board.get_z_key() && tt_hit.hash_move.get_raw_data() != 0) {
        type2collision++;
    }
    
    if (depth == 0) {
//            return quiescence_search(alpha, beta);
        int eval = board.static_eval();
        return eval;
    }
     
    
    MoveList moves;
    board.generate_moves(moves);
    
    // Check for checkmate and stalemate
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
    
    unsigned int node_type = NODE_UPPERBOUND;
    while (!move_picker.finished()) {
        int eval;
        auto it = ++move_picker;
        
        nodes_searched++;
        board.make_move(it);
        eval = -negamax(depth - 1, -beta, -alpha);
        board.unmake_move();
        
        
        if (eval >= beta) {
            board.tt.set(board.get_z_key(), Move(), depth, NODE_LOWERBOUND, beta, board.tt_sanity_check());
            return beta;
        }
        if (eval > alpha) {
            node_type = NODE_EXACT;
            alpha = eval;
        }
    }
    
    
    // Write search data to transposition table
    board.tt.set(board.get_z_key(), Move(), depth, node_type, alpha, board.tt_sanity_check());

    return alpha;
}


Move Search::find_best_move(unsigned int depth) {
    type2collision = 0;
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
        
//        std::cout << "\nMove: ";
//        print_move(it, true);
//        std::cout << "\nEval: " << eval;

        if (eval > maxEval) {
            maxEval = eval;
            best_move = it;
        }
        if (eval >= 2000000) {
            break;
        }
    }
    
    std::cout << "\nType2 Collisions: " << type2collision << "\n";
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
    
    TT_entry tt_hit = board.tt.get(board.get_z_key());
    
    if (tt_hit.key == board.get_z_key() && tt_hit.hash_move.get_depth() == depth) {
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
    board.tt.set(board.get_z_key(), Move(), depth, NODE_EXACT, nodes, board.tt_sanity_check());
    
    return nodes;
}

