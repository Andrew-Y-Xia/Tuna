//
//  Board.hpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef Board_hpp
#define Board_hpp

#include "depend.hpp"
#include "Bitboard.hpp"
#include "Board.hpp"
#include "Data_structs.hpp"
#include "Utility.hpp"
#include "Evaluation.hpp"
#include "Transposition_table.hpp"

void set_single_texture(int color, old::piece_type piece, sf::Sprite& sprite);


class Board {
private:
    // Handles board posititions:
    U64 Bitboards[8];
    
    
    // 0 is white, 1 is black
    int current_turn;
   
    // Castling states:
    bool white_can_castle_queenside;
    bool white_can_castle_kingside;
    bool black_can_castle_queenside;
    bool black_can_castle_kingside;
    
    int en_passant_square;
    
    // Move counters
    int halfmove_counter;
    int fullmove_counter;
    
    // Incrementally updated move values
    int piece_values[2];
    
    // This flag is not guaranteed to be correct! Guaranteed to be correct only after calling generate_moves()
    bool king_is_in_check;
    
    std::vector<move_data> move_stack;
    
    U64 z_key;
    
public:
    TT tt;
    
    Board();
    Board(std::string str);
    void read_FEN(std::string str);
    void standard_setup();
    void extracted();
    
    void hash();
    void init_zobrist_key();
    
    bool get_current_turn();
    
    void print_board();
    void print_z_key();
    
    unsigned int find_piece_occupying_sq(int index);
    unsigned int find_piece_captured(int index);
    unsigned int find_piece_captured_without_occ(int index);
    
    U64 get_positive_ray_attacks(int from_square, Directions dir, U64 occ);
    U64 get_negative_ray_attacks(int from_square, Directions dir, U64 occ);
    
    U64 bishop_attacks(int from_index, U64 occ);
    U64 rook_attacks(int from_index, U64 occ);
    U64 xray_bishop_attacks(int from_index, U64 occ, U64 blockers);
    U64 xray_rook_attacks(int from_index, U64 occ, U64 blockers);
    
    U64 in_between_mask(int from_index, int to_index);
    
    // Move generation:
    void generate_moves(MoveList& moves);
    void generate_king_moves(MoveList& moves, U64 occ, U64 friendly_pieces, int king_index, int num_attackers);
    void generate_pawn_movesW(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners, U64 rook_pinned, U64 bishop_pinned, int king_index);
    void generate_pawn_movesB(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners, U64 rook_pinned, U64 bishop_pinned, int king_index);
    void generate_knight_moves(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, U64 rook_pinned, U64 bishop_pinned);
    void generate_bishop_moves(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners, U64 rook_pinned, U64 bishop_pinned, int king_index);
    void generate_rook_moves(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners, U64 rook_pinned, U64 bishop_pinned, int king_index);
    void generate_queen_moves(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners, U64 rook_pinned, U64 bishop_pinned, int king_index);
    
    // Legality portion
    U64 attacks_to(int index, U64 occ);
    int is_attacked(int index, U64 occ);
    U64 calculate_block_masks(U64 king_attacker);
    U64 calculate_bishop_pins(int* pinners, U64 occ, U64 friendly_pieces);
    U64 calculate_rook_pins(int* pinners, U64 occ, U64 friendly_pieces);
    // Move generation end
    
    
    void make_move(Move move);
    void unmake_move();
    

    void assign_move_scores(MoveList &moves);
    
    // Move ordering:
    
    void sort_moves(MoveList& moves);
    
    // Evaluations utils
    
    void calculate_piece_values();
    void print_piece_values();
    int static_eval();
    bool is_king_in_check();
    
    
    // UI:
    Move request_move(Move move);
    bool is_trying_to_promote(Move move);
    
    void set_texture_to_pieces();
    
    U64 get_z_key();
    U64 tt_sanity_check() {
        return Bitboards[0] | Bitboards[1];
    }
};



#endif /* Board_hpp */
