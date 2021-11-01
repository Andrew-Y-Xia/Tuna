//
//  Search.cpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Search.hpp"




int type2collision;

MovePicker::MovePicker(MoveList& init_moves) : moves(init_moves) {
    size = init_moves.size();
    visit_count = 0;
}

bool MovePicker::filter_for_pruning() {
    // Tells MovePicker to not iterate over moves designated for pruning
    // Returns whether any move has been pruned
    int index = 0;
    bool any_pruned = false;
    for (auto it = moves.begin(); it != moves.end(); ++it, ++index) {
        bool should_be_skipped = it->get_move_score() == PRUNE_MOVE_SCORE;
        if (should_be_skipped) {
            visited[index] = true;
            any_pruned = true;
            visit_count++;
        }
    }
    return any_pruned;
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


Search::Search(Board b, TT& t, OpeningBook& ob, TimeHandler& th) : board(b), tt(t), opening_book(ob), time_handler(th) {
    nodes_searched = 0;
}

template <bool use_history_heuristic>
void Search::assign_move_scores(MoveList &moves, HashMove hash_move, Move killers[2]) {
    unsigned int score;

    // Score all the moves
    for (auto it = moves.begin(); it != moves.end(); ++it) {
        score = 512;

        if (hash_move == (*it)) {
            it->set_move_score(1000);
            continue;
        }

        if (it->is_capture()) {
            score += 70;
            score += board.static_exchange_eval(*it) / 8;
        }
        else if (killers[0] == (*it) || killers[1] == (*it)) {
            score += 65;
        }
        else if (use_history_heuristic) {
            unsigned int hist_lookup = history_moves[board.get_current_turn()][it->get_from()][it->get_to()];
            score += bitscan_reverse(hist_lookup) * !!hist_lookup; // Takes a base2 log of hist_lookup
        }

        if (it->get_special_flag() == MOVE_PROMOTION) {
            score += 100;
        }

        // Placing piece at square attacked by pawn is stupid, so subtract from score if that happens
        /*
         if (pawn_attacks[current_turn][it->get_to()] & Bitboards[Pawns] & Bitboards[!current_turn]) {
         score -= piece_to_value_small[it->get_piece_moved()];
         }
         */


        assert(score <= 1023);
        it->set_move_score(score);
    }
}

template <bool use_delta_pruning>
void Search::assign_move_scores_quiescent(MoveList &moves, int eval, int alpha) {
    unsigned int score;

    // Score all the moves
    for (auto it = moves.begin(); it != moves.end(); ++it) {
        score = 512;

        int mvv_lva_result = Board::mvv_lva(*it);

        if (mvv_lva_result >= 0) {
            // Delta Pruning
            if (!use_delta_pruning || eval + mvv_lva_result + 2*PAWN_VALUE > alpha) {
                score += mvv_lva_result / 8;
            } else {
                score = PRUNE_MOVE_SCORE;
            }
        } else {
            int see_result = board.static_exchange_eval(*it);
            if (see_result >= 0 /*&& (!use_delta_pruning || eval + see_result + 2*PAWN_VALUE > alpha)*/) {
                score += see_result / 8;
            }
            else {
                score = PRUNE_MOVE_SCORE;
            }
        }

        assert(score <= 1023);
        it->set_move_score(score);
    }
}

std::vector<Move> Search::get_pv() {
    std::vector<Move> pv;
    while (true) {
        TT_result tt_result = tt.get(board.get_z_key());
        if (!tt_result.is_hit || tt_result.tt_entry.hash_move.get_node_type() != NODE_EXACT || board.has_repeated_once()) {
            break;
        }
        Move m = tt_result.tt_entry.hash_move.to_move();
        MoveList legal_moves;
        board.generate_moves(legal_moves);
        assert(legal_moves.contains(tt_result.tt_entry.hash_move.to_move()));
        pv.push_back(m);
        board.make_move(m);
    }
    for (auto i = pv.size(); i != 0; i--) {
        board.unmake_move();
    }
    return pv;
}

std::string print_move_vector(std::vector<Move> moves) {
    std::ostringstream s;
    for (auto it = moves.begin(); it != moves.end(); it++) {
        s << move_to_str(*it, true) << ' ';
    }
    std::string popped = s.str();
    popped.pop_back();
    return popped;
}


void Search::store_pos_result(HashMove best_move, unsigned int depth, unsigned int node_type, int score,
                              unsigned int ply_from_root) {
    if (score >= MINMATE) {
        score += ply_from_root; // MAXMATE - (distance from this position to mate)
    } else if (score <= -MINMATE) {
        score -= ply_from_root; // -(MAXMATE - (distance from this position to mate)
    }
    tt.set(board.get_z_key(), best_move, depth, node_type, score);
}


int Search::negamax(unsigned int depth, int alpha, int beta, unsigned int ply_from_root, unsigned int ply_extended, bool do_null_move) {
//    tt.prefetch(board.get_z_key());

    if (board.has_repeated_once() || board.has_drawn_by_fifty_move_rule()) {
        bool is_in_check_pre;
        int move_count = board.calculate_mobility(is_in_check_pre);
        if (is_in_check_pre && move_count == 0) {
            return -MAXMATE + ply_from_root;
        }
        return 0;
    }

    // Check for hits on the TT
    const TT_result tt_result = tt.get(board.get_z_key());

    if (tt_result.is_hit && tt_result.tt_entry.hash_move.get_depth() >= depth) {
//        if (tt_result.sanity_check != board.tt_sanity_check()) {
//            std::cout << "Transposition table type 1 collision\n";
//        }

        int score = tt_result.tt_entry.score;
        unsigned int node_type = tt_result.tt_entry.hash_move.get_node_type();
        if (score >= MINMATE) {
            score -= ply_from_root; // This gets MAXMATE - (distance between mate and root)
        } else if (score <= -MINMATE) {
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

    if (tt_result.is_hit && tt_result.tt_entry.hash_move.get_raw_data() != 0) {
        type2collision++;
    }

    if (depth == 0) {
        // Extension part
        if (EXTENSION_LIMIT && ply_extended < EXTENSION_LIMIT && board.is_in_check()) {
            return negamax(1, alpha, beta, ply_from_root, ply_extended + 1, false);
        }
        return quiescence_search(0, alpha, beta, ply_from_root);
    } else if (time_handler.should_stop()) {
        // Reset board to original state
        for (int i = ply_from_root; i != 0; i--) {
            if (board.get_move_stack().back().is_null_move) {
                board.unmake_null_move();
            } else {
                board.unmake_move();
            }
        }
        // Throw error to escape
        throw SearchTimeout();
    }


    MoveList moves;
    bool is_in_check;
    board.generate_moves(moves, is_in_check);

    // Check for checkmate and stalemate
    if (moves.size() == 0) {
        if (is_in_check) {
            return -MAXMATE + ply_from_root;
        } else {
            return 0;
        }
    }


    // Null move pruning
    if (USE_NULL_MOVE_PRUNING && do_null_move && !is_in_check) {
        if (depth > R) {
            board.make_null_move();
            int null_eval = -negamax(depth - 1 - R, -beta, -beta + 1, ply_from_root + 1, ply_extended, false);
            board.unmake_null_move();

            if (null_eval >= beta) {
                return beta;
            }
        }
    }

    HashMove move_to_assign;
    if (tt_result.is_hit) {
        move_to_assign = tt_result.tt_entry.hash_move;
    }
    assign_move_scores<true>(moves, move_to_assign, &killer_moves[depth][0]);

    bool do_pvs = depth > 2;


    MovePicker move_picker(moves);
    HashMove best_move;

    unsigned int node_type = NODE_UPPERBOUND;

    if (USE_PV_SEARCH && do_pvs) {
        int first_eval;
        auto first_move = ++move_picker;
        nodes_searched++;

        board.make_move(first_move);
        first_eval = -negamax(depth - 1, -beta, -alpha, ply_from_root + 1, ply_extended, true);
        board.unmake_move();

        if (first_eval >= beta) {
            store_pos_result(best_move, depth, NODE_LOWERBOUND, beta, ply_from_root);
            register_killers(ply_from_root, first_move);
            register_history_move(depth, first_move);
            return beta;
        }
        if (first_eval > alpha) {
            node_type = NODE_EXACT;
            best_move = first_move;
            alpha = first_eval;
        }
    }


    while (!move_picker.finished()) {
        int eval;
        auto it = ++move_picker;
        nodes_searched++;

        board.make_move(it);
        if (USE_PV_SEARCH && do_pvs) {
            // null window search
            eval = -negamax(depth - 1, -alpha - 1, -alpha, ply_from_root + 1, ply_extended, true);
            // Check if within bounds
            if (eval > alpha && eval < beta) {
                // If so, research with full window
                eval = -negamax(depth - 1, -beta, -alpha, ply_from_root + 1, ply_extended, true);
            }
        } else {
            eval = -negamax(depth - 1, -beta, -alpha, ply_from_root + 1, ply_extended, true);
        }
        board.unmake_move();


        if (eval >= beta) {
            store_pos_result(best_move, depth, NODE_LOWERBOUND, beta, ply_from_root);
            register_killers(ply_from_root, it);
            register_history_move(depth, it);
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

void Search::register_killers(unsigned int ply_from_root, Move move) {
    assert(move.get_raw_data());
    if (USE_KILLERS && move != killer_moves[ply_from_root][0] && move != killer_moves[ply_from_root][1]) {
        killer_moves[ply_from_root][0] = killer_moves[ply_from_root][1];
        killer_moves[ply_from_root][1] = move;
    }
}

void Search::register_history_move(unsigned int depth, Move move) {
    assert(move.get_raw_data());
    if (USE_HIST_HEURISTIC) {
        history_moves[board.get_current_turn()][move.get_from()][move.get_to()] += depth * depth;
    }
}



int Search::quiescence_search(unsigned int ply_from_horizon, int alpha, int beta, unsigned int ply_from_root) {
    MoveList moves; bool is_in_check;
    board.generate_moves<CAPTURES_ONLY>(moves, is_in_check);

    // If in check and there are no capture-evasions, do not allow quiescent search to stand_pat
    int stand_pat = moves.size() == 0 && is_in_check ? -MAXMATE + ply_from_root : board.static_eval();
    if (ply_from_horizon >= 5) {
        return stand_pat;
    }
    if (stand_pat >= beta) {
        return beta;
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }

    bool is_late_endgame = board.get_piece_values()[board.get_current_turn()] < KNIGHT_VALUE + BISHOP_VALUE;
    if (is_late_endgame || !USE_DELTA_PRUNING || is_in_check) {
        // Switch off delta pruning for late endgame
        assign_move_scores_quiescent<false>(moves, stand_pat, alpha);
    } else {
        assign_move_scores_quiescent<true>(moves, stand_pat, alpha);
    }
    MovePicker move_picker(moves);
    move_picker.filter_for_pruning();

    while (!move_picker.finished()) {
        int eval;
        auto it = ++move_picker;

        nodes_searched++;
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

void Search::log_search_info(int depth, int eval) {
    std::ostringstream buffer;
    buffer << "info ";
    buffer << "score cp " << eval;
    buffer << " depth " << depth;
    buffer << " nodes " << nodes_searched;
    buffer << " pv " << print_move_vector(get_pv());
    buffer << '\n';
    get_synced_cout().print(buffer.str());
}

void Search::search_finished_message(Move best_move, int depth, int eval) {
    log_search_info(depth, eval);
    std::ostringstream buffer;
    buffer << "bestmove " << move_to_str(best_move, true);
    buffer << '\n';
    get_synced_cout().print(buffer.str());
}


Move Search::find_best_move(unsigned int max_depth = MAX_DEPTH) {
    max_depth = std::min(max_depth, (unsigned int) MAX_DEPTH);
    board.hash();
    tt.increment_age();
    type2collision = 0;
    nodes_searched = 0;

    // Clear killers
    for (int i = 0; i < MAX_DEPTH; i++) {
        killer_moves[i][0] = Move();
        killer_moves[i][1] = Move();
    }
    // Clear history table
    for (int turn = 0; turn < 2; turn++) {
        for (int y = 0; y < 64; y++) {
            for (int x = 0; x < 64; x++) {
                history_moves[turn][y][x] = 0;
            }
        }
    }

    time_handler.start();

    // Check opening_book
    if (USE_BOOK && opening_book.can_use_book() && board.get_reg_starting_pos()) {
        Move book_move = opening_book.request(board.get_move_stack());
        if (!book_move.is_illegal()) {
            time_handler.stop();
            search_finished_message(book_move, 0, 0);
            return book_move;
        }
    }

    Move best_move; // Best verified move
    int max_eval; // Best verified score

    MoveList moves;
    board.generate_moves(moves);

    // Don't bother searching if there's one legal move
    if (moves.size() == 1) {
        search_finished_message(moves[0], 0, 0);
        return moves[0];
    }

    int expected_eval = 0;

    // Iterative deepening loop
    int depth;
    for (depth = 1; depth <= max_depth; depth++) {

        HashMove best_move_temp;
        best_move_temp = best_move;


        int upper_bound = 25;
        int lower_bound = 25;


        unsigned int times_researched = 0;

        // Aspiration window re-search loop
        while (true) {
            times_researched++;

//            std::cout << "Windows: ("  << alpha << ", " << (expected_eval + upper_bound) << ")\n";

            int alpha; // Best score for this search
            int beta;
            Move local_best_move; // Best move for this search

            if (!USE_ASPIRATION_WINDOWS) {
                upper_bound = MAXMATE + 1; // If not using aspiration windows, set it to -inf
                lower_bound = MAXMATE + 1; // Same for beta, this time +inf
                expected_eval = 0;
            }

            alpha = expected_eval - lower_bound;
            beta = expected_eval + upper_bound;

            assign_move_scores<true>(moves, best_move_temp, &killer_moves[0][0]);
            MovePicker move_picker(moves);

            if (USE_PV_SEARCH) {
                int first_eval;
                auto first_move = ++move_picker;
                nodes_searched++;

                board.make_move(first_move);
                try {
                    first_eval = -negamax(depth - 1, -beta, -alpha, 1, 0, true);
                } catch (SearchTimeout& e) {
                    search_finished_message(best_move, depth - 1, max_eval);
                    time_handler.stop();
                    return best_move;
                }
                board.unmake_move();

                if (first_eval >= beta) {
                    // This will cause the rest of the moves to be skipped
                    assert(USE_ASPIRATION_WINDOWS); // beta cutoff should only occur in aspirated search
                    alpha = first_eval;
                    while (!move_picker.finished()) {
                        ++move_picker;
                    }
                    register_killers(0, first_move);
                    register_history_move(depth, first_move);
                }
                if (first_eval > alpha) {
                    local_best_move = first_move;
                    alpha = first_eval;
                }
            }

            while (!move_picker.finished()) {
                int eval;
                auto it = ++move_picker;

                nodes_searched++;
                board.make_move(it);
                try {
                    if (USE_PV_SEARCH) {
                        // null window search
                        eval = -negamax(depth - 1, -alpha - 1, -alpha, 1, 0, true);
                        // Check if within bounds
                        if (eval > alpha && eval < beta) {
                            // If so, research with full window
                            eval = -negamax(depth - 1, -beta, -alpha, 1, 0, true);
                        }
                    } else {
                        eval = -negamax(depth - 1, -beta, -alpha, 1, 0, true);
                    }
                } catch (SearchTimeout& e) {
                    Move m;
                    if (alpha <= expected_eval - lower_bound || alpha >= expected_eval + upper_bound) {
                        m = best_move;
                    }
                    else {
                        m = local_best_move;
                    }
                    search_finished_message(m, depth - 1, max_eval);
                    time_handler.stop();
                    return m;
                }
                board.unmake_move();

                if (eval >= beta) {
                    // In case of fail-high break loop early
                    assert(USE_ASPIRATION_WINDOWS); // beta cutoff should only occur in aspirated search
                    alpha = eval;
                    register_killers(0, it);
                    register_history_move(depth, it);
                    break;
                }
                if (eval > alpha) {
                    alpha = eval;
                    local_best_move = it;
                }
            } // Move picker loop

            // Check if score is within bounds
            if (alpha >= expected_eval + upper_bound) {
                // If so, do a re-search
                assert(USE_ASPIRATION_WINDOWS); // Should not fail when not using asp_windows
                if (times_researched >= 4) {
                    // If we've searched too many times and there's still no viable result, give up and widen bounds all the way
                    expected_eval = 0;
                    upper_bound = MAXMATE + 1;
                    lower_bound = MAXMATE + 1;
                } else {
                    upper_bound *= 4;
                }
            } else if (alpha <= expected_eval - lower_bound) {
                assert(USE_ASPIRATION_WINDOWS); // Should not fail when not using asp_windows
                if (times_researched >= 4) {
                    expected_eval = 0;
                    upper_bound = MAXMATE + 1;
                    lower_bound = MAXMATE + 1;
                } else {
                    lower_bound *= 4;
                }
            } else {
                // Search didn't fail high or fail low, so continue on to next stage of iterative deepening
                expected_eval = alpha;
                max_eval = alpha;
                best_move = local_best_move;
                break;
            }
        }

        // In the case of finding checkmate, end search early

        HashMove h_best_move;
        h_best_move = best_move;
        store_pos_result(h_best_move, depth, NODE_EXACT, max_eval, 0);

        // If we've found the shortest possible checkmate, exit early
        if (max_eval >= MINMATE && MAXMATE - max_eval <= depth) {
            search_finished_message(best_move, depth, max_eval);
            time_handler.stop();
            return best_move;
        }

        // Send this iteration's info to the gui
        log_search_info(depth, max_eval);
    }

    search_finished_message(best_move, max_depth, max_eval);
    time_handler.stop();
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
    Move blank[2];
    board.generate_moves(moves);
    assign_move_scores(moves, HashMove(), blank);

    MovePicker move_picker(moves);
    while (!move_picker.finished()) {
        auto it = ++move_picker;

        board.make_move(it);
        nodes += sort_perft(depth - 1);
        board.unmake_move();
    }

    return nodes;
}

long Search::hash_perft(unsigned int depth) {
    tt.clear();
    long result = hash_perft_internal(depth);
    tt.clear();
    return result;
}

long Search::hash_perft_internal(unsigned int depth) {
    if (depth == 0) {
        return 1;
    }

    TT_result tt_result = tt.get(board.get_z_key());

    if (tt_result.is_hit && tt_result.tt_entry.hash_move.get_depth() == depth) {
//        if (tt_result.sanity_check != board.tt_sanity_check()) {
//            std::cout << "Type 1 collision occured" << std::endl;
//        }
//        std::cout << tt_result.sanity_check << ' ' << board.tt_sanity_check() << '\n';
        return tt_result.tt_entry.score;
    }

    long nodes = 0;

    MoveList moves;
    board.generate_moves(moves);


    for (auto it = moves.begin(); it != moves.end(); ++it) {
        board.make_move(*it);
        nodes += hash_perft_internal(depth - 1);
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
