//
//  Search.cpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Search.hpp"

unsigned int lmr_values[256];

int type2collision;

void init_search() {
    for (unsigned int i = 0; i < 256; i++) {
        if (i < 3) {
            lmr_values[i] = 0;
        }
        /*
        else if (i >= 2 && i < 8) {
            lmr_values[i] = i / 4;
        }
            */
        else {
            lmr_values[i] = 1;
        }
    }

    /*
    for (unsigned int i = 0; i < 256; i++) {
        std::cout << lmr_values[i] << '\n';
    }
    */

}

MovePicker::MovePicker(MoveList& init_moves) : moves(init_moves) {
    size = init_moves.size();
    visit_count = 0;
}


inline int MovePicker::finished() {
    return visit_count == size;
}

inline MoveReturn MovePicker::operator++() {
    unsigned int highest_score = 0;
    int highest_index = 0;

    for (int i = visit_count; i < size; i++) {
        if (moves.get_move_score(i) > highest_score) {
             highest_index = i;
             highest_score = moves.get_move_score(i);
        }
    }

    std::swap(moves[visit_count], moves[highest_index]);
    std::swap(moves.get_move_score(visit_count), moves.get_move_score(highest_index));
    Move move = moves[visit_count];
    bool should_skip = !!moves.get_move_score(visit_count);
    visit_count++;
    return MoveReturn { move, should_skip };
}


Search::Search(Board b, TT& t, OpeningBook& ob, TimeHandler& th) : board(b), tt(t), opening_book(ob), time_handler(th) {
    nodes_searched = 0;
}

template<bool use_history_heuristic>
void Search::assign_move_scores(MoveList& moves, HashMove hash_move, Move killers[2]) {
    unsigned int score;

    int index = 0;
    // Score all the moves
    for (auto it = moves.begin(); it != moves.end(); ++it, ++index) {
        score = 200000000;

        if (hash_move == (*it)) {
            moves.get_move_score(index) = UINT32_MAX;
            continue;
        }

        if (it->is_capture()) {
            score += 70 * 1000000;
            score += (board.static_exchange_eval(*it) / 8) * 1000000;
        } else if (killers[0] == (*it) || killers[1] == (*it)) {
            score += 65 * 1000000;
        } else if (use_history_heuristic) {
            unsigned int hist_lookup = history_moves[board.get_current_turn()][it->get_from()][it->get_to()];
            score += hist_lookup; // Takes a base2 log of hist_lookup
        }

        if (it->get_special_flag() == MOVE_PROMOTION) {
            score += 100 * 1000000;
        }

        // Placing piece at square attacked by pawn is stupid, so subtract from score if that happens
        /*
         if (pawn_attacks[current_turn][it->get_to()] & Bitboards[Pawns] & Bitboards[!current_turn]) {
         score -= piece_to_value_small[it->get_piece_moved()];
         }
         */


//        assert(score <= 1023);
        moves.get_move_score(index) = score;
    }
}

template<bool use_delta_pruning>
void Search::assign_move_scores_quiescent(MoveList& moves, int eval, int alpha) {
    unsigned int score;

    int index = 0;
    // Score all the moves
    for (auto it = moves.begin(); it != moves.end(); ++it, ++index) {
        score = 512;

        int mvv_lva_result = Board::mvv_lva(*it);

        if (mvv_lva_result >= 0) {
            // Delta Pruning
            if (!use_delta_pruning || eval + mvv_lva_result + 2 * PAWN_VALUE > alpha) {
                score += mvv_lva_result / 8;
            } else {
                score = PRUNE_MOVE_SCORE;
            }
        } else {
            int see_result = board.static_exchange_eval(*it);
            if (see_result >= 0 /*&& (!use_delta_pruning || eval + see_result + 2*PAWN_VALUE > alpha)*/) {
                score += see_result / 8;
            } else {
                score = PRUNE_MOVE_SCORE;
            }
        }

        assert(score <= 1023);
        moves.get_move_score(index) = score;
    }
}

std::vector<Move> Search::get_pv() {
    std::vector<Move> pv;
    while (true) {
        TT_result tt_result = tt.get(board.get_z_key());
        if (!tt_result.is_hit || tt_result.tt_entry.hash_move.get_node_type() != NODE_EXACT ||
            board.has_repeated_once()) {
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


int Search::negamax(unsigned int depth, int alpha, int beta, unsigned int ply_from_root, unsigned int ply_extended,
                    bool do_null_move) {
//    tt.prefetch(board.get_z_key());

//    unsigned int original_depth = depth;

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
    if (USE_NULL_MOVE_PRUNING && do_null_move && !is_in_check && !board.possible_zugzwang()) {
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
        assert(move_to_assign.get_raw_data() != 0);
    }
    assign_move_scores<true>(moves, move_to_assign, &killer_moves[ply_from_root][0]);

    bool do_pvs = depth > 2;


    MovePicker move_picker(moves);
    HashMove best_move;

    unsigned int node_type = NODE_UPPERBOUND;

    // For tactical stability, do not reduce moves when in check
    const bool do_lmr = !is_in_check && depth > 2 && beta-alpha <= 1;
    unsigned int* lmr_value_ptr = lmr_values;

    if (USE_PV_SEARCH && do_pvs) {
        int first_eval;
        auto first_move = (++move_picker).move;
        nodes_searched++;
        lmr_value_ptr++;

        board.make_move(first_move);
        first_eval = -negamax(depth - 1, -beta, -alpha, ply_from_root + 1, ply_extended, true);
        board.unmake_move();

        if (first_eval >= beta) {
            best_move = first_move;
            assert(best_move.get_raw_data() != 0);
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
        unsigned int effective_depth = depth;
        auto it = (++move_picker).move;
        unsigned int depth_reduction_value = *lmr_value_ptr;
        lmr_value_ptr++;

        nodes_searched++;

        board.make_move(it);

        effective_depth = determine_depth(effective_depth, depth_reduction_value, it, do_lmr);

        pvs_lmr_core(alpha, beta, ply_from_root, ply_extended, do_pvs, eval, effective_depth, depth);

        board.unmake_move();

        if (eval >= beta) {
            best_move = it;
            assert(best_move.get_raw_data() != 0);
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
    assert(best_move.get_raw_data() != 0 || node_type == NODE_UPPERBOUND);
    store_pos_result(best_move, depth, node_type, alpha, ply_from_root);

    return alpha;
}

unsigned int Search::determine_depth(unsigned int effective_depth, unsigned int depth_reduction_value, Move move, bool do_lmr) {
    // Don't do lmr if move is tactical (capture, promotion)
    // Don't reduce when move gives check
    if (USE_LATE_MOVE_REDUCTION && do_lmr && !move.is_capture() && move.get_special_flag() == MOVE_NORMAL &&
        !board.is_in_check()) {
        effective_depth = std::max((effective_depth - depth_reduction_value) * (effective_depth >= depth_reduction_value), 1U);
    }
    return effective_depth;
}

void
Search::pvs_lmr_core(int alpha, int beta, unsigned int ply_from_root, unsigned int ply_extended, bool do_pvs, int& eval,
                     unsigned int effective_depth, unsigned int depth) {
    // Both PVS and LMR
    if (USE_PV_SEARCH && do_pvs) {
        // null window search with reduced depth
        eval = -negamax(effective_depth - 1, -alpha - 1, -alpha, ply_from_root + 1, ply_extended, true);
        // Check if within bounds
        if (eval > alpha && eval < beta) {
            // If so, research with full window with normal depth
            eval = -negamax(depth - 1, -beta, -alpha, ply_from_root + 1, ply_extended, true);
        }
    }
    // LMR only
    else {
        // Search with reduced depth
        eval = -negamax(effective_depth - 1, -beta, -alpha, ply_from_root + 1, ply_extended, true);
        // Nodes that raise alpha must be re-searched if depth was reduced
        if (eval > alpha && eval < beta && effective_depth != depth) {
            // Research with full window with normal depth
            eval = -negamax(depth - 1, -beta, -alpha, ply_from_root + 1, ply_extended, true);
        }
    }
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
    MoveList moves;
    bool is_in_check;
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

    while (!move_picker.finished()) {
        int eval;
        auto move_info = ++move_picker;
        auto it = move_info.move;

        // Skip moves marked for pruning
        if (move_info.should_skip) {
            continue;
        }

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

    bool is_in_check;
    MoveList moves;
    board.generate_moves(moves, is_in_check);

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

        bool do_pvs = depth > 2;


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


            unsigned int* lmr_value_ptr = lmr_values;
            const bool do_lmr = !is_in_check && depth > 2 && beta-alpha <= 1;

            if (USE_PV_SEARCH && do_pvs) {
                int first_eval;
                auto first_move = (++move_picker).move;
                nodes_searched++;
                lmr_value_ptr++;

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
                unsigned int effective_depth = depth;
                auto it = (++move_picker).move;
                unsigned int depth_reduction_value = *lmr_value_ptr;
                lmr_value_ptr++;

                nodes_searched++;
                board.make_move(it);

                effective_depth = determine_depth(effective_depth, depth_reduction_value, it, do_lmr);

                try {
                    pvs_lmr_core(alpha, beta, 0, 0, do_pvs, eval, effective_depth, depth);
                } catch (SearchTimeout& e) {
                    Move m;
                    // Check if alpha is currently in aspiration window
                    // If it is, take the current best move; else take the last confirmed best move
                    if (alpha <= expected_eval - lower_bound || alpha >= expected_eval + upper_bound) {
                        m = best_move;
                    } else {
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
        auto it = (++move_picker).move;

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
