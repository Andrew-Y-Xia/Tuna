//
//  Bitboard.cpp
//  Bitboard Chess
//
//  Created by Andy on 4/21/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Bitboard.hpp"

U64 rays[8][64];
U64 king_paths[64];
U64 knight_paths[64];
U64 pawn_attacks[2][64];
Directions direction_between[64][64];

U64 rook_rays[64];
U64 bishop_rays[64];


U64 eastOne(U64 b) { return (b << 1) & ~a_file; }

U64 noEaOne(U64 b) { return (b << 9) & ~a_file; }

U64 soEaOne(U64 b) { return (b >> 7) & ~a_file; }

U64 westOne(U64 b) { return (b >> 1) & ~h_file; }

U64 soWeOne(U64 b) { return (b >> 9) & ~h_file; }

U64 noWeOne(U64 b) { return (b << 7) & ~h_file; }


/**
 * generalized bitScan
 * @author Gerd Isenberg
 * @param bb bitboard to scan
 * @precondition bb != 0
 * @param reverse, true bitScanReverse, false bitScanForward
 * @return index (0..63) of least/most significant one bit
 */
int bitScan(U64 bb, bool reverse) {
    U64 rMask;
    assert (bb != 0);
    rMask = -(U64) reverse;
    bb &= -bb | rMask;
    return bitscan_reverse(bb);
}

void print_BB(U64 b) {
    const U64 c = 1;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            std::cout << !!(b & (c << (8 * (7 - y) + x))) << ' ';
        }
        std::cout << '\n';
    }
}

void print_ls1bs(U64 x) {
    while (x) {
        U64 ls1b = x & -x; // isolate LS1B
        print_BB(ls1b);
        std::cout << '\n';
        x &= x - 1; // reset LS1B
    }
}


void init_bitboard_utils() {
    init_king_paths();
    init_knight_paths();
    init_rays();
    init_pawn_attacks();
    init_direction_between();
}


void init_king_paths() {
    for (int index = a1; index <= h8; index++) {
        Cords c = index_to_cords(index);
        U64 mask = EmptyBoard;
        for (int y = -1; y < 2; y++) {
            for (int x = -1; x < 2; x++) {
                if (!(x == 0 && y == 0) && is_within_bounds(c.x + x, c.y + y)) {
                    mask |= C64(1) << cords_to_index(c.x + x, c.y + y);
                }
            }
        }
        king_paths[index] = mask;
    }
}


void init_knight_paths() {
    for (int index = a1; index <= h8; index++) {
        Cords c = index_to_cords(index);
        U64 mask = EmptyBoard;
        for (int i = -1; i < 2; i += 2) {
            for (int j = -1; j < 2; j += 2) {

                if (is_within_bounds(c.x + i * 2, c.y + j * 1)) {
                    mask |= C64(1) << cords_to_index(c.x + i * 2, c.y + j * 1);
                }
                if (is_within_bounds(c.x + j * 1, c.y + i * 2)) {
                    mask |= C64(1) << cords_to_index(c.x + j * 1, c.y + i * 2);
                }
            }
        }
        knight_paths[index] = mask;
    }
}


void init_rays() {
    U64 nort = C64(0x0101010101010100);
    for (int sq = 0; sq < 64; sq++, nort <<= 1) {
        rays[North][sq] = nort;
    }

    U64 noea = C64(0x8040201008040200);
    for (int f = 0; f < 8; f++, noea = eastOne(noea)) {
        U64 ne = noea;
        for (int r8 = 0; r8 < 64; r8 += 8, ne <<= 8) {
            rays[NorthEast][r8 + f] = ne;
        }
    }

    U64 nowe = a8_h1_diagonal ^ (C64(1) << 7);
    for (int f = 7; f >= 0; f--, nowe = westOne(nowe)) {
        U64 nw = nowe;
        for (int r8 = 0; r8 < 64; r8 += 8, nw <<= 8) {
            rays[NorthWest][r8 + f] = nw;
        }
    }

    U64 ea = first_rank ^ C64(1);
    for (int f = 0; f < 8; f++, ea = eastOne(ea)) {
        U64 ne = ea;
        for (int r8 = 0; r8 < 64; r8 += 8, ne <<= 8) {
            rays[East][r8 + f] = ne;
        }
    }

    U64 sout = C64(0x0080808080808080);
    for (int sq = 63; sq >= 0; sq--, sout >>= 1) {
        rays[South][sq] = sout;
    }

    U64 soea = a8_h1_diagonal ^ (C64(1) << 56);
    for (int f = 7; f >= 0; f--, soea = eastOne(soea)) {
        U64 se = soea;
        for (int r8 = 63; r8 >= 0; r8 -= 8, se >>= 8) {
            rays[SouthEast][r8 - f] = se;
        }
    }

    U64 sowe = a1_h8_diagonal ^ (C64(1) << 63);
    for (int f = 0; f < 8; f++, sowe = westOne(sowe)) {
        U64 sw = sowe;
        for (int r8 = 63; r8 >= 0; r8 -= 8, sw >>= 8) {
            rays[SouthWest][r8 - f] = sw;
        }
    }

    U64 wes = first_rank ^ (C64(1) << 7);
    for (int f = 7; f >= 0; f--, wes = westOne(wes)) {
        U64 we = wes;
        for (int r8 = 0; r8 < 64; r8 += 8, we <<= 8) {
            rays[West][r8 + f] = we;
        }
    }


    // Init rook_rays and bishop_rays

    for (int sq = 0; sq < 64; sq++) {
        U64 edge1, edge2, edge3, edge4;
        U64 r_rays = rays[North][sq] | rays[East][sq] | rays[South][sq] | rays[West][sq];
        U64 sq_BB = C64(1) << sq;

        edge1 = sq_BB & a_file ? C64(0) : a_file;
        edge2 = sq_BB & h_file ? C64(0) : h_file;
        edge3 = sq_BB & first_rank ? C64(0) : first_rank;
        edge4 = sq_BB & eighth_rank ? C64(0) : eighth_rank;

        r_rays &= ~(edge1 | edge2 | edge3 | edge4);

        rook_rays[sq] = r_rays;
    }

    for (int sq = 0; sq < 64; sq++) {
        U64 edge1, edge2, edge3, edge4;
        U64 b_rays = rays[NorthEast][sq] | rays[SouthEast][sq] | rays[SouthWest][sq] | rays[NorthWest][sq];

        edge1 = a_file;
        edge2 = h_file;
        edge3 = first_rank;
        edge4 = eighth_rank;

        b_rays &= ~(edge1 | edge2 | edge3 | edge4);

        bishop_rays[sq] = b_rays;
    }
}

void init_pawn_attacks() {
    // 1st rank and 8th rank pawn attacks are actually essential 
    for (int index = a1; index <= h8; index++) {
        pawn_attacks[WhitePieces][index] = (((C64(1) << index) << 9) & ~a_file) | (((C64(1) << index) << 7) & ~h_file);
        pawn_attacks[BlackPieces][index] = (((C64(1) << index) >> 9) & ~h_file) | (((C64(1) << index) >> 7) & ~a_file);
    }
}

void init_direction_between() {
    for (int from_index = a1; from_index <= h8; from_index++) {
        for (int to_index = a1; to_index <= h8; to_index++) {
            Cords from_c = index_to_cords(from_index);
            Cords to_c = index_to_cords(to_index);
            int d_x = sgn(to_c.x - from_c.x);
            int d_y = sgn(to_c.y - from_c.y);

            if (d_x == -1 && d_y == -1) {
                direction_between[from_index][to_index] = NorthWest;
            } else if (d_x == -1 && d_y == 0) {
                direction_between[from_index][to_index] = West;
            } else if (d_x == -1 && d_y == 1) {
                direction_between[from_index][to_index] = SouthWest;
            } else if (d_x == 0 && d_y == -1) {
                direction_between[from_index][to_index] = North;
            } else if (d_x == 0 && d_y == 1) {
                direction_between[from_index][to_index] = South;
            } else if (d_x == 1 && d_y == -1) {
                direction_between[from_index][to_index] = NorthEast;
            } else if (d_x == 1 && d_y == 0) {
                direction_between[from_index][to_index] = East;
            } else if (d_x == 1 && d_y == 1) {
                direction_between[from_index][to_index] = SouthEast;
            }
        }
    }
}
