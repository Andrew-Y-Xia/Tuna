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
#include "Transposition_table.hpp"
#include "Opening_book.hpp"
#include "Time_handler.hpp"

#define USE_NULL_MOVE_PRUNING 1
#define R 2

class MovePicker {
private:
    MoveList& moves;
    bool visited[256] = {false};
    int size, visit_count;
public:
    MovePicker(MoveList& init_moves);

    int finished();

    Move operator++();
};


struct SearchTimeout : public std::exception {
};


class Search {
private:
    Board board;
    TT& tt;
    OpeningBook& opening_book;
    TimeHandler& time_handler;

    unsigned int nodes_searched;
public:

    Search(Board b, TT& t, OpeningBook& ob, TimeHandler& th);

    void store_pos_result(HashMove best_move, unsigned int depth, unsigned int node_type, int score,
                          unsigned int ply_from_root);

    int negamax(unsigned int depth, int alpha, int beta, unsigned int ply_from_root, bool do_null_move);

    int quiescence_search(unsigned int ply_from_horizon, int alpha, int beta, unsigned int ply_from_root);

    Move find_best_move(unsigned int max_depth);

    long perft(unsigned int depth);

    long sort_perft(unsigned int depth);

    long hash_perft(unsigned int depth);

    long hash_perft_internal(unsigned int depth);

    long capture_perft(unsigned int depth);

};

#endif /* Search_hpp */
