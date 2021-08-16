//
//  Ray_gen.cpp
//  Bitboard Chess
//
//  Created by Andrew Xia on 8/6/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Ray_gen.hpp"

const U64 RMagic[64] = {
        0xa8002c000108020ULL,
        0x6c00049b0002001ULL,
        0x100200010090040ULL,
        0x2480041000800801ULL,
        0x280028004000800ULL,
        0x900410008040022ULL,
        0x280020001001080ULL,
        0x2880002041000080ULL,
        0xa000800080400034ULL,
        0x4808020004000ULL,
        0x2290802004801000ULL,
        0x411000d00100020ULL,
        0x402800800040080ULL,
        0xb000401004208ULL,
        0x2409000100040200ULL,
        0x1002100004082ULL,
        0x22878001e24000ULL,
        0x1090810021004010ULL,
        0x801030040200012ULL,
        0x500808008001000ULL,
        0xa08018014000880ULL,
        0x8000808004000200ULL,
        0x201008080010200ULL,
        0x801020000441091ULL,
        0x800080204005ULL,
        0x1040200040100048ULL,
        0x120200402082ULL,
        0xd14880480100080ULL,
        0x12040280080080ULL,
        0x100040080020080ULL,
        0x9020010080800200ULL,
        0x813241200148449ULL,
        0x491604001800080ULL,
        0x100401000402001ULL,
        0x4820010021001040ULL,
        0x400402202000812ULL,
        0x209009005000802ULL,
        0x810800601800400ULL,
        0x4301083214000150ULL,
        0x204026458e001401ULL,
        0x40204000808000ULL,
        0x8001008040010020ULL,
        0x8410820820420010ULL,
        0x1003001000090020ULL,
        0x804040008008080ULL,
        0x12000810020004ULL,
        0x1000100200040208ULL,
        0x430000a044020001ULL,
        0x280009023410300ULL,
        0xe0100040002240ULL,
        0x200100401700ULL,
        0x2244100408008080ULL,
        0x8000400801980ULL,
        0x2000810040200ULL,
        0x8010100228810400ULL,
        0x2000009044210200ULL,
        0x4080008040102101ULL,
        0x40002080411d01ULL,
        0x2005524060000901ULL,
        0x502001008400422ULL,
        0x489a000810200402ULL,
        0x1004400080a13ULL,
        0x4000011008020084ULL,
        0x26002114058042ULL,
};

const U64 BMagic[64] = {
        0x89a1121896040240ULL,
        0x2004844802002010ULL,
        0x2068080051921000ULL,
        0x62880a0220200808ULL,
        0x4042004000000ULL,
        0x100822020200011ULL,
        0xc00444222012000aULL,
        0x28808801216001ULL,
        0x400492088408100ULL,
        0x201c401040c0084ULL,
        0x840800910a0010ULL,
        0x82080240060ULL,
        0x2000840504006000ULL,
        0x30010c4108405004ULL,
        0x1008005410080802ULL,
        0x8144042209100900ULL,
        0x208081020014400ULL,
        0x4800201208ca00ULL,
        0xf18140408012008ULL,
        0x1004002802102001ULL,
        0x841000820080811ULL,
        0x40200200a42008ULL,
        0x800054042000ULL,
        0x88010400410c9000ULL,
        0x520040470104290ULL,
        0x1004040051500081ULL,
        0x2002081833080021ULL,
        0x400c00c010142ULL,
        0x941408200c002000ULL,
        0x658810000806011ULL,
        0x188071040440a00ULL,
        0x4800404002011c00ULL,
        0x104442040404200ULL,
        0x511080202091021ULL,
        0x4022401120400ULL,
        0x80c0040400080120ULL,
        0x8040010040820802ULL,
        0x480810700020090ULL,
        0x102008e00040242ULL,
        0x809005202050100ULL,
        0x8002024220104080ULL,
        0x431008804142000ULL,
        0x19001802081400ULL,
        0x200014208040080ULL,
        0x3308082008200100ULL,
        0x41010500040c020ULL,
        0x4012020c04210308ULL,
        0x208220a202004080ULL,
        0x111040120082000ULL,
        0x6803040141280a00ULL,
        0x2101004202410000ULL,
        0x8200000041108022ULL,
        0x21082088000ULL,
        0x2410204010040ULL,
        0x40100400809000ULL,
        0x822088220820214ULL,
        0x40808090012004ULL,
        0x910224040218c9ULL,
        0x402814422015008ULL,
        0x90014004842410ULL,
        0x1000042304105ULL,
        0x10008830412a00ULL,
        0x2520081090008908ULL,
        0x40102000a0a60140ULL,
};

U64 bishop_move_table[64][1024];
U64 rook_move_table[64][4096];


U64 get_positive_ray_attacks(int from_square, Directions dir, U64 occ) {
    // Gets ray attacks in directions North, NorthEast, NorthWest, East
    U64 attacks = rays[dir][from_square];
    U64 blockers = attacks & occ;
    int blocker = bitscan_forward(blockers | C64(0x8000000000000000));
    return attacks ^ rays[dir][blocker];
}

U64 get_negative_ray_attacks(int from_square, Directions dir, U64 occ) {
    // Gets ray attacks in directions West, SouthWest, South, SouthEast
    U64 attacks = rays[dir][from_square];
    U64 blockers = attacks & occ;
    int blocker = bitscan_reverse(blockers | C64(1));
    return attacks ^ rays[dir][blocker];
}

U64 bishop_attacks_classical(int from_index, U64 occ) {
    // Combines appropriate ray attacks for diagonal attacks
    return get_positive_ray_attacks(from_index, NorthEast, occ) | get_positive_ray_attacks(from_index, NorthWest, occ) |
           get_negative_ray_attacks(from_index, SouthEast, occ) | get_negative_ray_attacks(from_index, SouthWest, occ);
}

U64 rook_attacks_classical(int from_index, U64 occ) {
    // Combines appropiate ray attacks for rank-and-file attacks
    return get_positive_ray_attacks(from_index, North, occ) | get_positive_ray_attacks(from_index, East, occ) |
           get_negative_ray_attacks(from_index, South, occ) | get_negative_ray_attacks(from_index, West, occ);
}

U64 bishop_attacks(int from_index, U64 occ) {
    int bits = pop_count(bishop_rays[from_index]);
    U64 blockers = bishop_rays[from_index] & occ;
    U64 key = (blockers * BMagic[from_index]) >> (64 - bits);
    return bishop_move_table[from_index][key];
}

U64 rook_attacks(int from_index, U64 occ) {
    int bits = pop_count(rook_rays[from_index]);
    U64 blockers = rook_rays[from_index] & occ;
    U64 key = (blockers * RMagic[from_index]) >> (64 - bits);
    return rook_move_table[from_index][key];
}

U64 xray_bishop_attacks(int from_index, U64 occ, U64 blockers) {
    // xray routines for diagonals
    U64 attacks = bishop_attacks(from_index, occ);
    blockers &= attacks;
    return attacks ^ bishop_attacks(from_index, occ ^ blockers);
}

U64 xray_rook_attacks(int from_index, U64 occ, U64 blockers) {
    // xray routines for rank-and-file
    U64 attacks = rook_attacks(from_index, occ);
    blockers &= attacks;
    return attacks ^ rook_attacks(from_index, occ ^ blockers);
}

U64 in_between_mask(int from_index, int to_index) {
    // Returns bitboard with squares between the two indices set
    // The to_index is also set
    // Undefined behavior if from_index and to_index cannot be connected by a ray
    Directions dir = direction_between[from_index][to_index];
    U64 ray_from = rays[dir][from_index];
    U64 ray_to = rays[dir][to_index];
    return ray_from ^ ray_to;
}


/* Generate a unique blocker board, given an index (0..2^bits) and the blocker mask
 * for the piece/square. Each index will give a unique blocker board.
 * Sourced from https://stackoverflow.com/questions/30680559/how-to-find-magic-bitboards
 */
U64 gen_blockerboard(int index, U64 blockermask) {
    /* Start with a blockerboard identical to the mask. */
    U64 blockerboard = blockermask;

    /* Loop through the blockermask to find the indices of all set bits. */
    int bitindex = 0;
    for (int i = 0; i < 64; i++) {
        /* Check if the i'th bit is set in the mask (and thus a potential blocker). */
        if (blockermask & (1ULL << i)) {
            /* Clear the i'th bit in the blockerboard if it's clear in the index at bitindex. */
            if (!(index & (1 << bitindex))) {
                blockerboard &= ~(1ULL << i); //Clear the bit.
            }
            /* Increment the bit index in the 0-4096 index, so each bit in index will correspond
             * to each set bit in blockermask. */
            bitindex++;
        }
    }
    return blockerboard;
}

void init_ray_gen() {
    // Init magic_BB stuff

    // Bishop
    for (int sq = 0; sq < 64; sq++) {
        int bits = pop_count(bishop_rays[sq]);
        for (int i = 0; i < (1 << bits); i++) {
            U64 blockers = gen_blockerboard(i, bishop_rays[sq]);
            U64 key = (blockers * BMagic[sq]) >> (64 - bits);
            bishop_move_table[sq][key] = bishop_attacks_classical(sq, blockers);
        }
    }
    // Rook
    for (int sq = 0; sq < 64; sq++) {
        int bits = pop_count(rook_rays[sq]);
        for (int i = 0; i < (1 << bits); i++) {
            U64 blockers = gen_blockerboard(i, rook_rays[sq]);
            U64 key = (blockers * RMagic[sq]) >> (64 - bits);
            rook_move_table[sq][key] = rook_attacks_classical(sq, blockers);
        }
    }
}
