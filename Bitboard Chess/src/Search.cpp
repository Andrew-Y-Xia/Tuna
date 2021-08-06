//
//  Search.cpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Search.hpp"


#define MAXMATE 2000000
#define MINMATE 1999000


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
    int found_index = 0;

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




Search::Search(Board b, TT& t, OpeningBook& ob) : board(b), tt(t), opening_book(ob) {}


void Search::store_pos_result(HashMove best_move, unsigned int depth, unsigned int node_type, int score, unsigned int ply_from_root) {
    if (score >= MINMATE) {
        score += ply_from_root; // MAXMATE - (distance from this position to mate)
    }
    else if (score <= -MINMATE) {
        score -= ply_from_root; // -(MAXMATE - (distance from this position to mate)
    }
    tt.set(board.get_z_key(), best_move, depth, node_type, score);
}



int Search::negamax(unsigned int depth, int alpha, int beta, unsigned int ply_from_root) {
    // Check for hits on the TT
    const TT_entry tt_hit = tt.get(board.get_z_key());
    
    if (tt_hit.key == board.get_z_key() && tt_hit.hash_move.get_depth() >= depth) {
//        if (tt_hit.sanity_check != board.tt_sanity_check()) {
//            std::cout << "Transposition table type 1 collision\n";
//        }
        
        int score = tt_hit.score;
        unsigned int node_type = tt_hit.hash_move.get_node_type();
        if (score >= MINMATE) {
            score -= ply_from_root; // This gets MAXMATE - (distance between mate and root)
        }
        else if (score <= -MINMATE) {
            score += ply_from_root; // This gets -(MAXMATE - (distance between mate and root))
        }
        
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
        return quiescence_search(0, alpha, beta, ply_from_root + 1);
//        int eval = board.static_eval();
//        return eval;
    }
    else if (depth >= 5) {
        // Check if time is up
        // If so, exit
        std::chrono::duration<double, std::milli> ms_double = std::chrono::high_resolution_clock::now() - start_time;
        if (ms_double.count() >= max_time_ms) {
            // Reset board to original state
            for (int i = ply_from_root; i != 0; i--) {
                board.unmake_move();
            }
            // Throw error to escape
            throw SearchTimeout();
        }
    }
     
    
    MoveList moves;
    board.generate_moves(moves);
    
    // Check for checkmate and stalemate
    if (moves.size() == 0) {
        if (board.is_king_in_check()) {
            return -MAXMATE + ply_from_root;
        }
        else {
            return 0;
        }
    }
    
    // Check for repetition
    if (board.has_repeated_once()) {
        return 0;
    }
    
    
//    board.sort_moves(moves);
    if (tt_hit.key == board.get_z_key()) {
        board.assign_move_scores(moves, tt_hit.hash_move);
    }
    else {
        board.assign_move_scores(moves, HashMove());
    }
    
    
    MovePicker move_picker(moves);
    HashMove best_move;
    
    unsigned int node_type = NODE_UPPERBOUND;
    while (!move_picker.finished()) {
        int eval;
        auto it = ++move_picker;
        
        nodes_searched++;
        board.make_move(it);
        eval = -negamax(depth - 1, -beta, -alpha, ply_from_root + 1);
        board.unmake_move();
        
        
        if (eval >= beta) {
            store_pos_result(best_move, depth, NODE_LOWERBOUND, beta, ply_from_root);
            return beta;
        }
        if (eval > alpha) {
            node_type = NODE_EXACT;
            best_move = it;
            alpha = eval;
        }
    }
    
    
    // Write search data to transposition table
    store_pos_result(best_move, depth, node_type, alpha, ply_from_root);

    return alpha;
}


int Search::quiescence_search(unsigned int ply_from_horizon, int alpha, int beta, unsigned int ply_from_root) {
    int stand_pat = board.static_eval();
    if (ply_from_horizon >= 7) {
        return stand_pat;
    }
    if (stand_pat >= beta) {
        return beta;
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }
    
    MoveList moves;
    board.generate_moves<CAPTURES_ONLY>(moves);
    
    board.assign_move_scores(moves, HashMove());
    
    MovePicker move_picker(moves);

    while (!move_picker.finished()) {
        int eval;
        auto it = ++move_picker;
        
        board.make_move(it);
        eval = -quiescence_search(ply_from_horizon + 1, -beta, -alpha, ply_from_root + 1);
        board.unmake_move();
        

        if (eval >= beta) {
            return beta;
        }
        if (eval > alpha) {
            alpha = eval;
        }
    }
    
    return alpha;
}


static void search_finished_message(int depth, unsigned int nodes_searched) {
    std::cout << "\nType2 Collisions: " << type2collision << "\n";
    std::cout << "\nNodes searched: " << nodes_searched << "\n";
    std::cout << "\nDepth searched: " << depth << "\n\n";
}

Move Search::find_best_move(unsigned int max_depth, double max_time_ms_input) {
    board.hash();
    max_time_ms = max_time_ms_input;
    type2collision = 0;
    nodes_searched = 0;
    
    start_time = std::chrono::high_resolution_clock::now();
    
    // Check opening_book
    if (opening_book.can_use_book() && board.get_reg_starting_pos()) {
        Move book_move = opening_book.request(board.get_move_stack());
        if (!book_move.is_illegal()) {
            return book_move;
        }
    }

    Move best_move; // Best verified move
    int max_eval; // Best verified score
    
    MoveList moves;
    board.generate_moves(moves);
    
    
    int expected_eval = 0;
    
    int depth;
    for (depth = 1; depth <= max_depth; depth++) {
        TT_entry tt_hit = tt.get(board.get_z_key());
    
        if (tt_hit.key == board.get_z_key()) {
            board.assign_move_scores(moves, tt_hit.hash_move);
        }
        else {
            board.assign_move_scores(moves, HashMove());
        }
    

        
        
        int upper_bound = 25;
        int lower_bound = 25;
        
        
        unsigned int times_researched = 0;
        
        while (1) {
            times_researched++;
            
            int local_max_eval = expected_eval - lower_bound; // Best score for this search
            Move local_best_move; // Best move for this search
//            std::cout << "Windows: ("  << local_max_eval << ", " << (expected_eval + upper_bound) << ")\n";
            
            MovePicker move_picker(moves);
    
            while (!move_picker.finished()) {
                int eval;
    
    
                auto it = ++move_picker;
    
                nodes_searched++;
                board.make_move(it);
                try {
                    eval = -negamax(depth - 1, -(expected_eval + upper_bound), -local_max_eval, 1);
                } catch(SearchTimeout& e) {
                    search_finished_message(depth - 1, nodes_searched);
                    return best_move;
                }
                board.unmake_move();
    
//                std::cout << "\nMove: ";
//                print_move(it, true);
//                std::cout << "\nEval: " << eval;

                if (eval > local_max_eval) {
                    local_max_eval = eval;
                    local_best_move = it;
                }
            }
            
            // Check if score is within bounds
            if (local_max_eval >= expected_eval + upper_bound) {
                // If so, do a re-search
                if (times_researched >= 4) {
                    // If we've searched too many times and there's still no viable result, give up and widen bounds all the way
                    expected_eval = 0;
                    upper_bound = MAXMATE + 1;
                    lower_bound = MAXMATE + 1;
                }
                else {
                    upper_bound *= 4;
                }
            }
            else if (local_max_eval <= expected_eval - lower_bound) {
                if (times_researched >= 4) {
                    expected_eval = 0;
                    upper_bound = MAXMATE + 1;
                    lower_bound = MAXMATE + 1;
                }
                else {
                    lower_bound *= 4;
                }
            }
            else {
                // Continue on to next stage of iterative deepening
                expected_eval = local_max_eval;
                max_eval = local_max_eval;
                best_move = local_best_move;
                break;
            }
        }
        
        // In the case of finding checkmate, end search early
    
        HashMove h_best_move;
        h_best_move = best_move;
        store_pos_result(h_best_move, depth, NODE_EXACT, max_eval, 0);
        
        if (max_eval >= MINMATE && MAXMATE - max_eval <= depth) {
            search_finished_message(depth, nodes_searched);
            return best_move;
        }
    }
    
    search_finished_message(max_depth, nodes_searched);
    return best_move;
}


long Search::perft(unsigned int depth) {
    
    if (depth == 0) {
//        print_board();
        return 1;
    }
    
    long nodes = 0;

    MoveList moves;
    board.generate_moves(moves);


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

    MoveList moves;
    board.generate_moves(moves);
    board.assign_move_scores(moves, HashMove());

    MovePicker move_picker(moves);
    while (!move_picker.finished()) {
        auto it = ++move_picker;
        
        board.make_move(it);
        nodes += sort_perft(depth-1);
        board.unmake_move();
    }
    
    return nodes;
}

// TODO: add TT clear
long Search::hash_perft(unsigned int depth) {
    if (depth == 0) {
        return 1;
    }
    
    TT_entry tt_hit = tt.get(board.get_z_key());
    
    if (tt_hit.key == board.get_z_key() && tt_hit.hash_move.get_depth() == depth) {
//        if (tt_hit.sanity_check != board.tt_sanity_check()) {
//            std::cout << "Type 1 collision occured" << std::endl;
//        }
//        std::cout << tt_hit.sanity_check << ' ' << board.tt_sanity_check() << '\n';
        return tt_hit.score;
    }

    long nodes = 0;

    MoveList moves;
    board.generate_moves(moves);


    for (auto it = moves.begin(); it != moves.end(); ++it) {
        board.make_move(*it);
        nodes += hash_perft(depth - 1);
        board.unmake_move();
    }
    
    // Write data to transposition table
    tt.set(board.get_z_key(), Move(), depth, NODE_EXACT, (int) nodes);
    
    return nodes;
}


long Search::capture_perft(unsigned int depth) {
    int captures = 0;
    
    if (depth == 0) {
//        print_board();
        return 1;
    }
    
    long nodes = 0;

    MoveList moves;
    board.generate_moves(moves);
    
    MoveList capture_moves;
    board.generate_moves<CAPTURES_ONLY>(capture_moves);


    for (auto it = moves.begin(); it != moves.end(); ++it) {
        board.make_move(*it);
        nodes += capture_perft(depth - 1);
        board.unmake_move();
        
        if (it->is_capture()) {
            captures++;
        }
    }
    
    assert(captures == capture_moves.size());

    return nodes;
}
