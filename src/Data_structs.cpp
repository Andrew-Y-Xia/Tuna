//
//  Data_structs.cpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Data_structs.hpp"

bool Cords::operator==(const Cords c2) {
    return (this->x == c2.x && this->y == c2.y);
}

bool Cords::operator!=(const Cords c2) {
    return !(*this == c2);
}

Cords::Cords() {
    x = -1;
    y = -1;
}

Cords::Cords(int a, int b) {
    x = a;
    y = b;
}


// Move BEGIN

Move::Move() {
    move_data = 0;
}

Move::Move(unsigned int from, unsigned int to, unsigned int flag, unsigned int promotion_piece_or_castle_type,
           unsigned int piece_moved, unsigned int piece_captured, unsigned int score) {
    move_data =
            (from & 0x3F) | ((to & 0x3F) << 6) | ((flag & 0x3) << 12) | ((promotion_piece_or_castle_type & 0x3) << 14) |
            ((piece_moved & 0x7) << 16) | ((piece_captured & 0x7) << 19) | ((score & 0x3FF) << 22);
}

Move::Move(unsigned int raw_data) {
    move_data = raw_data;
}

unsigned int Move::get_raw_data() const {
    return move_data;
}

void Move::set_raw_data(unsigned int data) {
    move_data = data;
}

bool Move::operator==(Move& a) {
    return (move_data & 0xFFFF) == (a.get_raw_data() & 0xFFFF);
}

bool Move::operator!=(Move& a) {
    return (move_data & 0xFFFF) != (a.get_raw_data() & 0xFFFF);
}

unsigned int Move::get_from() const {
    return move_data & 0x3F;
}

unsigned int Move::get_to() const {
    return (move_data >> 6) & 0x3F;
}

unsigned int Move::get_special_flag() const {
    return (move_data >> 12) & 0x3;
}

unsigned int Move::get_promote_to() const {
    return (move_data >> 14) & 0x3;
}

unsigned int Move::get_castle_type() const {
    return (move_data >> 14) & 0x3;
}

unsigned int Move::get_piece_moved() const {
    return (move_data >> 16) & 0x7;
}

unsigned int Move::get_piece_captured() const {
    return (move_data >> 19) & 0x7;
}

unsigned int Move::get_move_score() const {
    return (move_data >> 22) & 0x3FF;
}

void Move::set_from(unsigned int from) {
    move_data &= ~(0x3F);
    move_data |= from & 0x3F;
}

void Move::set_to(unsigned int to) {
    move_data &= ~(0x3F << 6);
    move_data |= (to & 0x3F) << 6;
}

void Move::set_special_flag(unsigned int flag) {
    move_data &= ~(0x3 << 12);
    move_data |= (flag & 0x3) << 12;
}

void Move::set_promote_to(unsigned int piece) {
    move_data &= ~(0x3 << 14);
    move_data |= (piece & 0x3) << 14;
}

void Move::set_castle_type(unsigned int castle_type) {
    move_data &= ~(0x3 << 14);
    move_data |= (castle_type & 0x3) << 14;
}

void Move::set_piece_moved(unsigned int piece) {
    move_data &= ~(0x7 << 16);
    move_data |= (piece & 0x7) << 16;
}

void Move::set_piece_captured(unsigned int piece) {
    move_data &= ~(0x7 << 19);
    move_data |= (piece & 0x7) << 19;
}

void Move::set_move_score(unsigned int score) {
    assert(score <= 1023);
    move_data &= ~(0x3FF << 22);
    move_data |= (score & 0x3FF) << 22;
}


bool Move::is_capture() {
    return move_data & (0x7 << 19);
}

bool Move::is_illegal() {
    return get_piece_moved() == PIECE_EXTRA;
}

void Move::set_as_illegal() {
    set_piece_moved(PIECE_EXTRA);
}


MoveList::MoveList() {
    back_index = 0;
}

void MoveList::push_back(Move move) {
    moves[back_index] = move;
    back_index++;
}

Move* MoveList::begin() {
    return moves;
}

Move* MoveList::end() {
    return moves + back_index;
}

int MoveList::size() {
    return back_index;
}

bool MoveList::contains(Move move) {
    for (auto it = begin(); it != end(); ++it) {
        if (*it == move) {
            return true;
        }
    }
    return false;
}

Move& MoveList::operator[](int index) {
    return moves[index];
}


bool move_cmp(Move first, Move second) {
    return first.get_move_score() > second.get_move_score();
}

bool Move::first_twelfth_eq(Move& a) {
    return (move_data & 0xFFF) == (a.get_raw_data() & 0xFFF);
}

// Move END
