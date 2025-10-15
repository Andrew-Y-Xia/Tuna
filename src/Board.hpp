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
#include "Ray_gen.hpp"
#include "Zobrist.hpp"

// Forward declarations for NNUE
namespace NNUE {
    struct Accumulator;
}

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

    int piece_square_values_m[2];
    int piece_square_values_e[2];

//    int piece_count[2][6];

    // NNUE accumulator for incremental updates
    NNUE::Accumulator* nnue_accumulator;

    std::vector<move_data> move_stack;

    bool reg_starting_pos;

    U64 z_key;

public:
    Board();

    Board(std::string str);

    void read_FEN(std::string str);

    Move read_SAN(std::string str);

    Move read_LAN(std::string str);

    void standard_setup();

    void hash();

    bool get_current_turn() const;

    void print_board();

    void print_z_key();

    bool verify_bitboard();

    unsigned int find_piece_occupying_sq(int index) const;
    
    bool is_white_piece(int index) const;

    unsigned int find_piece_captured(int index);

    unsigned int find_piece_captured_without_occ(int index);

    // Move generation:
    template<MoveGenType gen_type, SerializationType serialize_type>
    int generate_moves_inner(MoveList& moves, bool& is_in_check);

    template<MoveGenType gen_type = ALL_MOVES>
    void generate_moves(MoveList& moves, bool& is_in_check);

    template<MoveGenType gen_type = ALL_MOVES>
    void generate_moves(MoveList& moves);

    template<MoveGenType gen_type = ALL_MOVES>
    int calculate_mobility();

    template<MoveGenType gen_type = ALL_MOVES>
    int calculate_mobility(bool& is_in_check);

    template<MoveGenType gen_type, SerializationType serialize_type>
    int generate_king_moves(MoveList& moves, U64 occ, U64 friendly_pieces, int king_index, int num_attackers);

    template<MoveGenType gen_type, SerializationType serialize_type>
    int generate_pawn_movesW(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners,
                              U64 rook_pinned, U64 bishop_pinned, int king_index);

    template<MoveGenType gen_type, SerializationType serialize_type>
    int generate_pawn_movesB(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners,
                              U64 rook_pinned, U64 bishop_pinned, int king_index);

    template<MoveGenType gen_type, SerializationType serialize_type>
    int generate_knight_moves(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, U64 rook_pinned,
                               U64 bishop_pinned);

    template<MoveGenType gen_type, SerializationType serialize_type>
    int generate_bishop_moves(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners,
                               U64 rook_pinned, U64 bishop_pinned, int king_index);

    template<MoveGenType gen_type, SerializationType serialize_type>
    int generate_rook_moves(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners,
                             U64 rook_pinned, U64 bishop_pinned, int king_index);

    template<MoveGenType gen_type, SerializationType serialize_type>
    int generate_queen_moves(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners,
                              U64 rook_pinned, U64 bishop_pinned, int king_index);

    // Legality portion
    U64 attacks_to(int index, U64 occ);

    int is_attacked(int index, U64 occ);

    U64 get_att_def(int index, U64 occ);

    U64 calculate_block_masks(U64 king_attacker);

    U64 calculate_bishop_pins(int* pinners, U64 occ, U64 friendly_pieces);

    U64 calculate_rook_pins(int* pinners, U64 occ, U64 friendly_pieces);

    bool is_in_check();

    // Move generation end


    void make_move(Move move);

    void unmake_move();

    void make_null_move();

    void unmake_null_move();

    // Move ordering:

    U64 get_least_valuable_piece(U64 att_def, unsigned int by_side, unsigned int &piece);

    int static_exchange_eval(Move move);

    static int mvv_lva(Move move);

    // Evaluations utils

    void calculate_piece_values();

    int* get_piece_values();

    void print_piece_values();

    void calculate_piece_square_values();

    void print_piece_square_values();

    int calculate_game_phase();

    int calculate_pawn_shield_bonus();

    int static_eval();

    bool has_repeated_once();

    bool has_repeated_twice();

    bool has_drawn_by_fifty_move_rule();

    bool possible_zugzwang();


    // UI:
    Move request_move(Move move);

    bool is_trying_to_promote(Move move);

    // Produce info
    U64 get_z_key();

    std::vector<move_data> get_move_stack();

    bool get_reg_starting_pos();
    
    // NNUE accumulator access
    NNUE::Accumulator* get_nnue_accumulator() const;
    void refresh_nnue_accumulator();

    void serialize_promotion(MoveList &moves, int from_index, int to_index, unsigned int piece_captured) const;
};


#endif /* Board_hpp */
