//
//  Board.cpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//
#include "Utility.hpp"
#include "Board.hpp"
#include "Bitboard.hpp"

extern sf::Texture textures[13];
extern std::forward_list<sf::Sprite> sprites, promotion_sprites_white, promotion_sprites_black;
extern int incre8[8];


Board::Board() {
    read_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Board::Board(std::string str) {
    read_FEN(str);
}


void Board::read_FEN(std::string str) {
    
    for (int i = WhitePieces; i <= Pawns; i++) {
        Bitboards[i] = EmptyBoard;
    }
    
    int state_flag = 0;

    std::string en_passant_square, halfmove_str, fullmove_str;

    int x = 0;
    int y = 0;

    for (std::string::iterator it=str.begin(); it!=str.end(); ++it) {
        if (*it == ' ') {
            state_flag += 1;
        }
        else if (state_flag == 0) {
            if (*it == '/') {
                x = 0;
                y += 1;
//                    std::cout << "y increment" << '\n';
            }
            else {
                if (isdigit(*it)) {
                    int blanks = *it - '0';
                    x += blanks;
                }
                else {
                    if (isupper(*it)) {
                        // Set to white
                        Bitboards[WhitePieces] |= C64(1) << cords_to_index(x, y);
                    }
                    else {
                        // Set to black
                        Bitboards[BlackPieces] |= C64(1) << cords_to_index(x, y);
                    }

                    switch ((char) tolower(*it)) {
                        case 'r':
                            Bitboards[Rooks] |= C64(1) << cords_to_index(x, y);
                            break;
                        case 'b':
                            Bitboards[Bishops] |= C64(1) << cords_to_index(x, y);
                            break;
                        case 'n':
                            Bitboards[Knights] |= C64(1) << cords_to_index(x, y);
                            break;
                        case 'k':
                            Bitboards[Kings] |= C64(1) << cords_to_index(x, y);
                            break;
                        case 'q':
                            Bitboards[Queens] |= C64(1) << cords_to_index(x, y);
                            break;
                        case 'p':
                            Bitboards[Pawns] |= C64(1) << cords_to_index(x, y);
                            break;
                        default:
                            std::cout << "This should not have been reached. Invalid piece: " << (char) tolower(*it) <<'\n';
                    }
                    
                    // std::cout << squares[y][x].piece << '\n';
                    x++;
                }
            }
        }
        else if (state_flag == 1) {
            if (*it == 'w') {
                current_turn = 0;
            }
            else {
                current_turn = 1;
            }
        }
        else if (state_flag == 2) {
            if (*it == '-') {
                white_can_castle_queenside = false;
                white_can_castle_kingside = false;
                black_can_castle_queenside = false;
                black_can_castle_kingside = false;
            }
            else if (*it == 'K') {
                white_can_castle_kingside = true;
            }
            else if (*it == 'Q') {
                white_can_castle_queenside = true;
            }
            else if (*it == 'k') {
                black_can_castle_kingside = true;
            }
            else if (*it == 'q') {
                black_can_castle_queenside = true;
            }
        }
        else if (state_flag == 3) {
            en_passant_square.append(1, *it);
        }
        else if (state_flag == 4) {
            halfmove_str.append(1, *it);
        }
        else if (state_flag == 5) {
            fullmove_str.append(1, *it);
        }
        else {
            std::cout << "This state should not have been reached. ReadLEN Error occured." << '\n';
        }
    }


    halfmove_counter = std::stoi(halfmove_str);
    fullmove_counter = std::stoi(fullmove_str);

    if (en_passant_square[0] != '-') {
        switch (en_passant_square[0]) {
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
                std::cout << "Should not have been reached. En Passant square cords are wrong";
        }
        y = 8 - (en_passant_square[1] - '0');
        
        this->en_passant_square = cords_to_index(x, y);
    }
    else {
        this->en_passant_square = -1;
    }
}

void Board::print_board() {
    std::cout << "\n\n\n\nWhitePieces:\n";
    print_BB(Bitboards[WhitePieces]);
    std::cout << "\n\nBlackPieces:\n";
    print_BB(Bitboards[BlackPieces]);
    std::cout << "\n\nKings:\n";
    print_BB(Bitboards[Kings]);
    std::cout << "\n\nQueens:\n";
    print_BB(Bitboards[Queens]);
    std::cout << "\n\nRooks:\n";
    print_BB(Bitboards[Rooks]);
    std::cout << "\n\nBishops:\n";
    print_BB(Bitboards[Bishops]);
    std::cout << "\n\nKnights:\n";
    print_BB(Bitboards[Knights]);
    std::cout << "\n\nPawns:\n";
    print_BB(Bitboards[Pawns]);
}


unsigned int Board::find_piece_captured(int index) {
    // This function does not check if the capture is illegal, make sure to &~friendly_pieces beforehand.
    // Also, it doesn't check king captures
    
    /*
    U64 bit = C64(1) << index;
    return 2 * ((Bitboards[Queens] & bit) != 0) + 3 * ((Bitboards[Rooks] & bit) != 0) + 4 * ((Bitboards[Bishops] & bit) != 0) + 5 * ((Bitboards[Knights] & bit) != 0) + 6 * ((Bitboards[Pawns] & bit) != 0);
     */

    U64 bit = C64(1) << index;
    if (!(Bitboards[!current_turn] & bit)) {
        return 0;
    }
    else if (Bitboards[Pawns] & bit) {
        return 6;
    }
    else if (Bitboards[Knights] & bit) {
        return 5;
    }
    else if (Bitboards[Bishops] & bit) {
        return 4;
    }
    else if (Bitboards[Rooks] & bit) {
        return 3;
    }
    else if (Bitboards[Queens] & bit) {
        return 2;
    }
    else {
        return 0;
    }
}


// MOVE GENERATION BEGIN

void Board::generate_moves(std::vector<Move>& moves) {
    generate_king_moves(moves);
    generate_knight_moves(moves);
}


void Board::generate_king_moves(std::vector<Move>& moves) {
    U64 king = Bitboards[Kings] & Bitboards[current_turn];
    
    int from_index = bitscan_forward(king);
    U64 move_targets = king_paths[from_index] & ~Bitboards[current_turn];
    
    if (move_targets) do {
        int to_index = bitscan_forward(move_targets);
        moves.push_back(Move(from_index, to_index, 0, 0, 1, find_piece_captured(to_index)));
    } while (move_targets &= move_targets - 1);
}


void Board::generate_knight_moves(std::vector<Move>& moves) {
    U64 knights = Bitboards[Knights] & Bitboards[current_turn];
    
    if (knights) do {
        int from_index = bitscan_forward(knights);
        U64 move_targets = knight_paths[from_index] & ~Bitboards[current_turn];
        if (move_targets) do {
            int to_index = bitscan_forward(move_targets);
            moves.push_back(Move(from_index, to_index, 0, 0, 5, find_piece_captured(to_index)));
        } while (move_targets &= move_targets - 1);
        
    } while (knights &= knights - 1);
}


// MOVE GENERATION END
