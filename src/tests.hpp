//
// Created by Andrew Xia on 9/15/21.
//

#ifndef BITBOARD_CHESS_TESTS_HPP
#define BITBOARD_CHESS_TESTS_HPP

#include "Board.hpp"
#include "Search.hpp"
#include "Transposition_table.hpp"
#include "Opening_book.hpp"
#include "Time_handler.hpp"

void test_perft(std::string fen, unsigned int depth, long result);

void tests();

#endif //BITBOARD_CHESS_TESTS_HPP
