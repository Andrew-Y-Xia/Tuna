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
#include "Board.hpp"
#include "Data_structs.hpp"
#include "Utility.hpp"


class Board {
private:
    // Handles board posititions:
    U64 Bitboards[8];
    
    
    // 0 is white, 1 is black
    int current_turn;
   
    // Castling states:
    bool white_can_castle_queenside: 1;
    bool white_can_castle_kingside: 1;
    bool black_can_castle_queenside: 1;
    bool black_can_castle_kingside: 1;
    
    int en_passant_square;
    
    // Move counters
    int halfmove_counter;
    int fullmove_counter;
    
    // Incrementally updated move values
    int black_piece_values, white_piece_values;
    
public:
    Board();
    Board(std::string str);
    void read_FEN(std::string str);
    void print_board();
    
    unsigned int find_piece_captured(int index);
    
    U64 get_positive_ray_attacks(int from_square, Directions dir, U64 occ);
    U64 get_negative_ray_attacks(int from_square, Directions dir, U64 occ);
    
    U64 bishop_attacks(int from_index, U64 occ);
    U64 rook_attacks(int from_index, U64 occ);
    U64 xray_bishop_attacks(int from_index, U64 occ, U64 blockers);
    U64 xray_rook_attacks(int from_index, U64 occ, U64 blockers);
    
    U64 in_between_mask(int from_index, int to_index);
    
    void generate_moves(std::vector<Move>& moves);
    void generate_pawn_movesW(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces);
    void generate_pawn_movesB(std::vector<Move>&, U64 block_check_masks, U64 occ, U64 friendly_pieces);
    void generate_king_moves(std::vector<Move>& moves, U64 occ, U64 friendly_pieces);
    void generate_knight_moves(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces);
    void generate_bishop_moves(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces);
    void generate_rook_moves(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces);
    void generate_queen_moves(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces);
    
    // Legality portion
    U64 attacks_to(int index, U64 occ);
    int is_attacked(int index, U64 occ);
    U64 calculate_block_masks(U64 king_attacker);
    U64 calculate_bishop_pins(int* pinners, U64 occ, U64 friendly_pieces);
    U64 calculate_rook_pins(int* pinners, U64 occ, U64 friendly_pieces);
};



#endif /* Board_hpp */
