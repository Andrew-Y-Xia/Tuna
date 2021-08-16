//
//  Evaluation.cpp
//  Bitboard Chess
//
//  Created by Andrew Xia on 5/6/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Evaluation.hpp"

int piece_to_value[8];
int piece_to_value_small[8];

// Piece square tables were yoinked from https://www.chessprogramming.org/PeSTO's_Evaluation_Function

int pawn_ps_m[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5, 5, 10, 25, 25, 10, 5, 5,
        0, 0, 0, 20, 20, 0, 0, 0,
        5, -5, -10, 0, 0, -10, -5, 5,
        5, 10, 10, -20, -20, 10, 10, 5,
        0, 0, 0, 0, 0, 0, 0, 0
};

int pawn_ps_e[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        80, 80, 80, 80, 80, 80, 80, 80,
        30, 40, 40, 45, 45, 40, 30, 30,
        5, 5, 10, 25, 25, 10, 5, 5,
        0, 0, 0, 20, 20, 0, 0, 0,
        5, 0, 0, 0, 0, 0, 0, 5,
        5, 10, 10, -20, -20, 10, 10, 5,
        0, 0, 0, 0, 0, 0, 0, 0
};

int knight_ps_m[64] = {
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20, 0, 0, 0, 0, -20, -40,
        -30, 0, 10, 15, 15, 10, 0, -30,
        -30, 5, 15, 20, 20, 15, 5, -30,
        -30, 0, 15, 20, 20, 15, 0, -30,
        -30, 5, 10, 15, 15, 10, 5, -30,
        -40, -20, 0, 5, 5, 0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50,
};

int knight_ps_e[64] = {
        -70, -60, -45, -45, -45, -45, -60, -70,
        -60, -45, 0, 0, 0, 0, -45, -60,
        -45, 0, 10, 15, 15, 10, 0, -45,
        -45, 5, 15, 20, 20, 15, 5, -45,
        -45, 0, 15, 20, 20, 15, 0, -45,
        -45, 5, 10, 15, 15, 10, 5, -45,
        -60, -45, 0, 5, 5, 0, -45, -60,
        -70, -60, -45, -45, -45, -45, -60, -70,
};

int bishop_ps_m[64] = {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 10, 10, 10, 10, 10, 10, -10,
        -10, 5, 0, 0, 0, 0, 5, -10,
        -20, -10, -10, -10, -10, -10, -10, -20,
};

int bishop_ps_e[64] = {
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 10, 10, 10, 10, 10, 10, -10,
        -10, 5, 0, 0, 0, 0, 5, -10,
        -20, -10, -10, -10, -10, -10, -10, -20,
};

int rook_ps_m[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, 10, 10, 10, 10, 5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        0, 0, 0, 5, 5, 0, 0, 0,
};

int rook_ps_e[64] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, 10, 10, 10, 10, 5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        5, 7, 7, 7, 7, 7, 7, 5,
        0, 0, 0, 0, 0, 0, 0, 0,
};

int queen_ps_m[64] = {
        -20, -10, -10, -5, -5, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -5, 0, 5, 5, 5, 5, 0, -5,
        0, 0, 5, 5, 5, 5, 0, -5,
        -10, 5, 5, 5, 5, 5, 0, -10,
        -10, 0, 5, 0, 0, 0, 0, -10,
        -20, -10, -10, -5, -5, -10, -10, -20,
};

int queen_ps_e[64] = {
        -20, -10, -10, -5, -5, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -5, 0, 5, 5, 5, 5, 0, -5,
        -5, 0, 5, 5, 5, 5, 0, -5,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, -5, -5, -5, -5, -5, -5, -10,
        -20, -10, -10, -5, -5, -10, -10, -20,
};

int king_ps_m[64] = {
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        20, 20, 0, 0, 0, 0, 20, 20,
        20, 30, 10, 0, 0, 10, 30, 20,
};

int king_ps_e[64] = {
        -50, -40, -30, -20, -20, -30, -40, -50,
        -30, -20, -10, 0, 0, -10, -20, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 30, 40, 40, 30, -10, -30,
        -30, -10, 20, 30, 30, 20, -10, -30,
        -30, -30, 0, 0, 0, 0, -30, -30,
        -50, -30, -30, -30, -30, -30, -30, -50,
};

int* ps_m[6] = {
        king_ps_m,
        queen_ps_m,
        rook_ps_m,
        bishop_ps_m,
        knight_ps_m,
        pawn_ps_m,
};

int* ps_e[6] = {
        king_ps_e,
        queen_ps_e,
        rook_ps_e,
        bishop_ps_e,
        knight_ps_e,
        pawn_ps_e,
};


U64 king_loc_queenside_castled[2] = {
        0x7ULL,
        0x700000000000000ULL,
};

U64 king_loc_kingside_castled[2] = {
        0xE0ULL,
        0xE000000000000000ULL,
};


U64 pawn_shield_queenside[2] = {
        0x70700ULL,
        0x7070000000000ULL,
};
U64 pawn_shield_kingside[2] = {
        0xE0E000ULL,
        0xE0E00000000000ULL,
};


void init_piece_to_value() {
    piece_to_value[PIECE_PAWN] = PAWN_VALUE;
    piece_to_value[PIECE_KNIGHT] = KNIGHT_VALUE;
    piece_to_value[PIECE_BISHOP] = BISHOP_VALUE;
    piece_to_value[PIECE_ROOK] = ROOK_VALUE;
    piece_to_value[PIECE_QUEEN] = QUEEN_VALUE;

    piece_to_value_small[PIECE_PAWN] = PAWN_VALUE / 10;
    piece_to_value_small[PIECE_KNIGHT] = KNIGHT_VALUE / 10;
    piece_to_value_small[PIECE_BISHOP] = BISHOP_VALUE / 10;
    piece_to_value_small[PIECE_ROOK] = ROOK_VALUE / 10;
    piece_to_value_small[PIECE_QUEEN] = QUEEN_VALUE / 10;
}

void init_piece_square_tables() {
    index_remap(pawn_ps_m);
    index_remap(pawn_ps_e);
    index_remap(knight_ps_m);
    index_remap(knight_ps_e);
    index_remap(bishop_ps_m);
    index_remap(bishop_ps_e);
    index_remap(rook_ps_m);
    index_remap(rook_ps_e);
    index_remap(queen_ps_m);
    index_remap(queen_ps_e);
    index_remap(king_ps_m);
    index_remap(king_ps_e);
}

void init_eval_utils() {
    init_piece_to_value();
    init_piece_square_tables();
}

void index_remap(int* array) {
    // Since board is shown in flipped order (for readibility), flip it back first
    int copy[64];
    for (unsigned int i = 0; i < 64; i++) {
        copy[i] = array[i];
    }
    for (unsigned int i = 0; i < 64; i++) {
        array[i] = copy[flip_index_v(i)];
    }
}

int lookup_ps_table_m(unsigned int index, unsigned int piece, int current_turn) {
    piece -= 2;

    if (current_turn == BLACK) {
        index = flip_index_v(index);
    }

    return ps_m[piece][index];
}

int lookup_ps_table_e(unsigned int index, unsigned int piece, int current_turn) {
    piece -= 2;

    if (current_turn == BLACK) {
        index = flip_index_v(index);
    }

    return ps_e[piece][index];
}
