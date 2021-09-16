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
} __attribute__ ((__packed__));


enum MoveGenType {
    ALL_MOVES,
    CAPTURES_ONLY,
};

enum SerializationType {
    SERIALIZE_MOVES,
    COUNT_MOVES,
};


struct Cords {
    int x: 8;
    int y: 8;

    bool operator==(const Cords c2);

    bool operator!=(const Cords c2);

    Cords();

    Cords(int a, int b);
};


#define MOVE_NORMAL 0
#define MOVE_PROMOTION 1
#define MOVE_ENPASSANT 2
#define MOVE_CASTLING 3

#define PROMOTE_TO_KNIGHT 3
#define PROMOTE_TO_BISHOP 2
#define PROMOTE_TO_ROOK 1
#define PROMOTE_TO_QUEEN 0

#define CASTLE_TYPE_KINGSIDE 0
#define CASTLE_TYPE_QUEENSIDE 1

#define PIECE_NONE 0
#define PIECE_EXTRA 1
#define PIECE_KING 2
#define PIECE_QUEEN 3
#define PIECE_ROOK 4
#define PIECE_BISHOP 5
#define PIECE_KNIGHT 6
#define PIECE_PAWN 7


struct Move {
protected:
    unsigned int move_data;
    /*
     
     Move contents:
     
     bits 0-5: move_from
     bits 6-11: move_to
     bits 12-13: special_move_flag: Normal(0), Promotion(1), En_passant(2), Castling(3)
     bits 14-15: promote_to: Queen(0), Rook(1), Bishop(2), Knight(3)
           union castle_type: Kingside(0), Queenside(1)
     bits 16-18: piece_moved: None(0), Extra(1), King(2), Queen(3), Rook(4), Bishop(5), Knight(6), Pawn(7)
     bits 19-21: piece_captured: None(0), Extra(1), King(2), Queen(3), Rook(4), Bishop(5), Knight(6), Pawn(7)
     bits 22-31: move score
     
    */

public:
    Move();

    Move(unsigned int from, unsigned int to, unsigned int flag, unsigned int promotion_piece_or_castle_type,
         unsigned int piece_moved, unsigned int piece_captured, unsigned int score = 0);

    unsigned int get_raw_data() const;

    void set_raw_data(unsigned int data);

    bool operator==(Move& a);

    bool operator!=(Move& a);


    unsigned int get_from() const;

    unsigned int get_to() const;

    unsigned int get_special_flag() const;

    unsigned int get_promote_to() const;

    unsigned int get_castle_type() const;

    unsigned int get_piece_moved() const;

    unsigned int get_piece_captured() const;

    unsigned int get_move_score() const;


    void set_from(unsigned int from);

    void set_to(unsigned int to);

    void set_special_flag(unsigned int flag);

    void set_promote_to(unsigned int piece);

    void set_castle_type(unsigned int piece);

    void set_piece_moved(unsigned int piece);

    void set_piece_captured(unsigned int piece);

    void set_move_score(unsigned int score);


    bool is_capture();

    bool is_illegal();

    void set_as_illegal();

    bool first_twelfth_eq(Move& a);

};


class MoveList {
private:
    Move moves[256];
    int back_index;
public:
    MoveList();

    void push_back(Move move);

    Move* begin();

    Move* end();

    int size();

    bool contains(Move move);

    Move& operator[](int index);
};

bool move_cmp(Move first, Move second);


struct move_data {
    Move move;

    bool white_can_castle_queenside;
    bool white_can_castle_kingside;
    bool black_can_castle_queenside;
    bool black_can_castle_kingside;

    int en_passant_square;

    U64 z_key;

    bool is_null_move;

    int halfmove_counter;
};


namespace old {
// What type of piece is it?
    enum piece_type {
        Empty,
        Pawn,
        Knight,
        Bishop,
        Rook,
        Queen,
        King
    };

    enum move_type {
        // Pawn moving two steps for initial move is not included since it can be treated like a normal move
        Normal,
        Promote_to_Queen,
        Promote_to_Rook,
        Promote_to_Bishop,
        Promote_to_Knight,
        En_Passant,
        Castle_Queenside,
        Castle_Kingside,
        Illegal
    };


    struct Move {
        Cords from_c;
        Cords to_c;

        move_type type: 4;
        int score: 8;
    };
} // old


#endif /* Data_structs_hpp */
