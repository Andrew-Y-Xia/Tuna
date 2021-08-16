//
//  Utility.cpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Utility.hpp"


unsigned int flip_index_v(unsigned int i) {
    return i ^ 56;
}

char num_to_char(int input) {
    char c;
    switch (input) {
        case 0:
            c = 'a';
            break;
        case 1:
            c = 'b';
            break;
        case 2:
            c = 'c';
            break;
        case 3:
            c = 'd';
            break;
        case 4:
            c = 'e';
            break;
        case 5:
            c = 'f';
            break;
        case 6:
            c = 'g';
            break;
        case 7:
            c = 'h';
            break;
        default:
            c = '\0';
    }
    return c;
}

int char_to_num(char input) {
    int x = 0;
    switch (input) {
        case 'a':
            x = 0;
            break;
        case 'b':
            x = 1;
            break;
        case 'c':
            x = 2;
            break;
        case 'd':
            x = 3;
            break;
        case 'e':
            x = 4;
            break;
        case 'f':
            x = 5;
            break;
        case 'g':
            x = 6;
            break;
        case 'h':
            x = 7;
            break;
        default:
            std::cout << "Should not have been reached. char_to_num";
    }
    return x;
}

unsigned int piece_char_to_piece(char input) {
    input = tolower(input);
    switch (input) {
        case 'k':
            return PIECE_KING;
        case 'q':
            return PIECE_QUEEN;
        case 'r':
            return PIECE_ROOK;
        case 'b':
            return PIECE_BISHOP;
        case 'n':
            return PIECE_KNIGHT;
        case 'p':
            return PIECE_PAWN;
        default:
            std::cout << "Piece is invalid\n";
            return 0;
    }
}

unsigned int txt_square_to_index(std::string str) {
    int x = char_to_num(str[0]);
    int y = 8 - (str[1] - '0');

    return cords_to_index(x, y);
}


int cords_to_index(int x, int y) {
    return 8 * (7 - y) + x;
}


Cords index_to_cords(int index) {
    Cords c;
    c.x = index % 8;
    c.y = 7 - (index / 8);
    return c;
}


void print_cords(Cords c) {
    std::cout << "Cords{" << c.x << ", " << c.y << '}';
}

bool is_within_bounds(int x, int y) {
    return (0 <= x && x <= 7 && 0 <= y && y <= 7);
}

void print_move(Move move, bool reg) {
    if (!reg) {
        std::cout << "\n\n\nFrom index: " << move.get_from() << '\n';
        std::cout << "To index: " << move.get_to() << '\n';
        std::cout << "Special move flag: " << move.get_special_flag() << '\n';
        std::cout << "Promote to piece: " << move.get_promote_to() << '\n';
        std::cout << "Piece moved: " << move.get_piece_moved() << '\n';
        std::cout << "Piece captured: " << move.get_piece_captured() << '\n';
        std::cout << "Move score: " << move.get_move_score() << '\n';
    } else {
        Cords from_c = index_to_cords(move.get_from());
        Cords to_c = index_to_cords(move.get_to());
        std::cout << num_to_char(from_c.x) << 8 - from_c.y << num_to_char(to_c.x) << 8 - to_c.y;
        if (move.get_special_flag() == MOVE_PROMOTION) {
            switch (move.get_promote_to()) {
                case PROMOTE_TO_QUEEN:
                    std::cout << 'q';
                    break;
                case PROMOTE_TO_ROOK:
                    std::cout << 'r';
                    break;
                case PROMOTE_TO_BISHOP:
                    std::cout << 'b';
                    break;
                case PROMOTE_TO_KNIGHT:
                    std::cout << 'n';
                    break;
            }
        }
    }
}

namespace converter {

    old::piece_type piece_type_to_old(unsigned int piece) {
        switch (piece) {
            case PIECE_NONE:
                return old::Empty;
            case PIECE_PAWN:
                return old::Pawn;
            case PIECE_KNIGHT:
                return old::Knight;
            case PIECE_BISHOP:
                return old::Bishop;
            case PIECE_ROOK:
                return old::Rook;
            case PIECE_QUEEN:
                return old::Queen;
            case PIECE_KING:
                return old::King;
            default:
                std::cout << "Something went wrong piece_type_to_old";
                return old::Empty;
        }
    }

    old::Move move_to_old(Move move) {
        old::Move old_move;
        old_move.from_c = index_to_cords(move.get_from());
        old_move.to_c = index_to_cords(move.get_to());
        switch (move.get_special_flag()) {
            case MOVE_NORMAL:
                old_move.type = old::Normal;
                break;
            case MOVE_CASTLING:
                if (move.get_castle_type() == CASTLE_TYPE_QUEENSIDE) {
                    old_move.type = old::Castle_Queenside;
                } else {
                    old_move.type = old::Castle_Kingside;
                }
                break;
            case MOVE_ENPASSANT:
                old_move.type = old::En_Passant;
                break;
            case MOVE_PROMOTION:
                switch (move.get_promote_to()) {
                    case PROMOTE_TO_KNIGHT:
                        old_move.type = old::Promote_to_Knight;
                        break;
                    case PROMOTE_TO_BISHOP:
                        old_move.type = old::Promote_to_Bishop;
                        break;
                    case PROMOTE_TO_ROOK:
                        old_move.type = old::Promote_to_Rook;
                        break;
                    case PROMOTE_TO_QUEEN:
                        old_move.type = old::Promote_to_Queen;
                        break;
                }
                break;
        }
        if (move.get_piece_moved() == PIECE_EXTRA) {
            old_move.type = old::Illegal;
        }
        return old_move;
    }

    unsigned int old_piece_type_to_new(old::piece_type piece) {
        switch (piece) {
            case old::Empty:
                return PIECE_NONE;
            case old::Pawn:
                return PIECE_PAWN;
            case old::Knight:
                return PIECE_KNIGHT;
            case old::Bishop:
                return PIECE_BISHOP;
            case old::Rook:
                return PIECE_ROOK;
            case old::Queen:
                return PIECE_QUEEN;
            case old::King:
                return PIECE_KING;
            default:
                std::cout << "Something went wrong old_piece_type_to_new";
                return PIECE_EXTRA;
        }
    }

    Move old_move_to_new(old::Move old_move) {
        Move new_move;

        new_move.set_from(cords_to_index(old_move.from_c.x, old_move.from_c.y));
        new_move.set_to(cords_to_index(old_move.to_c.x, old_move.to_c.y));

        switch (old_move.type) {
            case old::Normal:
                new_move.set_special_flag(MOVE_NORMAL);
                break;
            case old::Promote_to_Queen:
                new_move.set_special_flag(MOVE_PROMOTION);
                new_move.set_promote_to(PROMOTE_TO_QUEEN);
                break;
            case old::Promote_to_Rook:
                new_move.set_special_flag(MOVE_PROMOTION);
                new_move.set_promote_to(PROMOTE_TO_ROOK);
                break;
            case old::Promote_to_Bishop:
                new_move.set_special_flag(MOVE_PROMOTION);
                new_move.set_promote_to(PROMOTE_TO_BISHOP);
                break;
            case old::Promote_to_Knight:
                new_move.set_special_flag(MOVE_PROMOTION);
                new_move.set_promote_to(PROMOTE_TO_KNIGHT);
                break;
            case old::En_Passant:
                new_move.set_special_flag(MOVE_ENPASSANT);
                break;
            case old::Castle_Queenside:
                new_move.set_special_flag(MOVE_CASTLING);
                new_move.set_castle_type(CASTLE_TYPE_QUEENSIDE);
                break;
            case old::Castle_Kingside:
                new_move.set_special_flag(MOVE_CASTLING);
                new_move.set_castle_type(CASTLE_TYPE_KINGSIDE);
                break;
            case old::Illegal:
                new_move.set_piece_moved(PIECE_EXTRA);
                break;
        }

        return new_move;
    }


} // converter

