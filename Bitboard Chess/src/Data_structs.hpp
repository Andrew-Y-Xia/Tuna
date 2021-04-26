//
//  Data_structs.hpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#ifndef Data_structs_hpp
#define Data_structs_hpp

#include "depend.hpp"

enum square_mappings {
  a1 = 0, b1, c1, d1, e1, f1, g1, h1,
  a2, b2, c2, d2, e2, f2, g2, h2,
  a3, b3, c3, d3, e3, f3, g3, h3,
  a4, b4, c4, d4, e4, f4, g4, h4,
  a5, b5, c5, d5, e5, f5, g5, h5,
  a6, b6, c6, d6, e6, f6, g6, h6,
  a7, b7, c7, d7, e7, f7, g7, h7,
  a8, b8, c8, d8, e8, f8, g8, h8
};

enum Enum_BoardBB {
    WhitePieces = 0,
    BlackPieces,
    Kings,
    Queens,
    Rooks,
    Bishops,
    Knights,
    Pawns
};


enum Directions {
    North = 0,
    NorthEast,
    East,
    SouthEast,
    South,
    SouthWest,
    West,
    NorthWest
};


struct Cords {
    int x: 8;
    int y: 8;
    
    bool operator==(const Cords c2);
    bool operator!=(const Cords c2);
    Cords();
    Cords(int a, int b);
};



struct Move {
private:
    unsigned int move_data;
    /*
     
     Move contents:
     
     bits 0-5: move_from
     bits 6-11: move_to
     bits 12-13: special_move_flag: Normal(0), Promotion(1), En_passant(2), Castling(3)
     bits 14-15: promote_to: Knight(0), Bishop(1), Rook(2), Queen(3)
     bits 16-18: piece_moved: None(0), King(1), Queen(2), Rook(3), Bishop(4), Knight(5), Pawn(6)
     bits 19-21: piece_captured: None(0), King(1), Queen(2), Rook(3), Bishop(4), Knight(5), Pawn(6)
     bits 22-31: move score
     
    */
public:
    Move(unsigned int from, unsigned int to, unsigned int flag, unsigned int promotion_piece, unsigned int piece_moved, unsigned int piece_captured, unsigned int score = 0);
    
    void operator=(Move& a);
    bool operator==(Move& a);
    bool operator!=(Move& a);
    
    
    unsigned int get_from() const;
    unsigned int get_to() const;
    unsigned int get_special_flag() const;
    unsigned int get_promote_to() const;
    unsigned int get_piece_moved() const;
    unsigned int get_piece_captured() const;
    unsigned int get_move_score() const;
    
    void set_from(unsigned int from);
    void set_to(unsigned int to);
    void set_special_flag(unsigned int flag);
    void set_promote_to(unsigned int piece);
    void set_piece_moved(unsigned int piece);
    void set_piece_captured(unsigned int piece);
    void set_move_score(unsigned int score);
    
    bool is_capture();

};


#endif /* Data_structs_hpp */
