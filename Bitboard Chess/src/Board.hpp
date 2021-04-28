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
    
    U64 get_positive_ray_attacks(int from_square, Directions dir);
    U64 get_negative_ray_attacks(int from_square, Directions dir);
    
    U64 bishop_attacks(int from_index);
    U64 rook_attacks(int from_index);
    
    void generate_moves(std::vector<Move>& moves);
    void generate_pawn_movesW(std::vector<Move>& moves);
    void generate_pawn_movesB(std::vector<Move>& moves);
    void generate_king_moves(std::vector<Move>& moves);
    void generate_knight_moves(std::vector<Move>& moves);
    void generate_bishop_moves(std::vector<Move>& moves);
    void generate_rook_moves(std::vector<Move>& moves);
    void generate_queen_moves(std::vector<Move>& moves);
    
};



#endif /* Board_hpp */
