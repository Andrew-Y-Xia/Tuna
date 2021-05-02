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


U64 Board::get_positive_ray_attacks(int from_square, Directions dir, U64 occ) {
    U64 attacks = rays[dir][from_square];
    U64 blockers = attacks & occ;
    int blocker = bitscan_forward(blockers | C64(0x8000000000000000));
    return attacks ^ rays[dir][blocker];
}

U64 Board::get_negative_ray_attacks(int from_square, Directions dir, U64 occ) {
    U64 attacks = rays[dir][from_square];
    U64 blockers = attacks & occ;
    int blocker = bitscan_reverse(blockers | C64(1));
    return attacks ^ rays[dir][blocker];
}

U64 Board::bishop_attacks(int from_index, U64 occ) {
    return get_positive_ray_attacks(from_index, NorthEast, occ) | get_positive_ray_attacks(from_index, NorthWest, occ) | get_negative_ray_attacks(from_index, SouthEast, occ) | get_negative_ray_attacks(from_index, SouthWest, occ);
}

U64 Board::rook_attacks(int from_index, U64 occ) {
    return get_positive_ray_attacks(from_index, North, occ) | get_positive_ray_attacks(from_index, East, occ) | get_negative_ray_attacks(from_index, South, occ) | get_negative_ray_attacks(from_index, West, occ);
}

U64 Board::in_between_mask(int from_index, int to_index) {
    // Includes the to_index
    Directions dir = direction_between[from_index][to_index];
    U64 ray_from = rays[dir][from_index];
    U64 ray_to = rays[dir][to_index];
    return ray_from ^ ray_to;
}

// MOVE GENERATION BEGIN


void Board::generate_moves(std::vector<Move>& moves) {
    U64 king_attackers, bishop_pinned, rook_pinned;
    int num_attackers;
    U64 block_masks = 0;
    
    int pinners[8];
    
    U64 occ = Bitboards[WhitePieces] | Bitboards[BlackPieces];
    U64 friendly_pieces = Bitboards[current_turn];
    
    generate_king_moves(moves, occ, friendly_pieces);
    king_attackers = attacks_to(bitscan_forward(Bitboards[Kings] & friendly_pieces), occ);
    num_attackers = _mm_popcnt_u64(king_attackers);
    
    if (num_attackers == 1) {
        block_masks = calculate_block_masks(king_attackers);
    }
    else if (num_attackers > 1) {
        return;
    }
    
    
    bishop_pinned = calculate_bishop_pins(pinners, occ, friendly_pieces);
    rook_pinned = calculate_rook_pins(pinners, occ, friendly_pieces);

    if (current_turn == 0) {
        generate_pawn_movesW(moves, block_masks, occ, friendly_pieces);
    }
    else {
        generate_pawn_movesB(moves, block_masks, occ, friendly_pieces);
    }
    generate_knight_moves(moves, block_masks, occ, friendly_pieces);
    generate_bishop_moves(moves, block_masks, occ, friendly_pieces);
    generate_rook_moves(moves, block_masks, occ, friendly_pieces);
    generate_queen_moves(moves, block_masks, occ, friendly_pieces);
}

// TODO: Castling
void Board::generate_king_moves(std::vector<Move>& moves, U64 occ, U64 friendly_pieces) {
    U64 king = Bitboards[Kings] & friendly_pieces;
    
    int from_index = bitscan_forward(king);
    U64 move_targets = king_paths[from_index] & ~friendly_pieces;
    
    if (move_targets) do {
        int to_index = bitscan_forward(move_targets);
        if (!is_attacked(to_index, occ)) {
            moves.push_back(Move(from_index, to_index, MOVE_NORMAL, 0, 1, find_piece_captured(to_index)));
        }
    } while (move_targets &= move_targets - 1);
}

// TODO: En-passant
void Board::generate_pawn_movesW(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces) {
    
    U64 pawns = Bitboards[Pawns] & friendly_pieces;
    
    if (pawns) {
        // East attacks:
        U64 east_attacks = ((pawns << 9) & ~a_file) & Bitboards[BlackPieces];
        east_attacks &= block_check_masks;
        // filter out promotions
        U64 east_promotion_attacks = east_attacks & eighth_rank;
        east_promotion_attacks &= block_check_masks;
        U64 east_regular_attacks = east_attacks & ~eighth_rank;
        east_regular_attacks &= block_check_masks;
        
        
        // West attacks:
        U64 west_attacks = ((pawns << 7) & ~h_file) & Bitboards[BlackPieces];
        west_attacks &= block_check_masks;
        U64 west_promotion_attacks = west_attacks & eighth_rank;
        west_promotion_attacks &= block_check_masks;
        U64 west_regular_attacks = west_attacks & ~eighth_rank;
        west_regular_attacks &= block_check_masks;
        
        
        
        // Serialize into moves:
        if (east_regular_attacks) do {
            int to_index = bitscan_forward(east_regular_attacks);
            moves.push_back(Move(to_index-9, to_index, MOVE_NORMAL, 0, PIECE_PAWN, find_piece_captured(to_index)));
        } while (east_regular_attacks &= east_regular_attacks - 1);
        
        if (east_promotion_attacks) do {
            int to_index = bitscan_forward(east_promotion_attacks);
            moves.push_back(Move(to_index-9, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, find_piece_captured(to_index)));
            moves.push_back(Move(to_index-9, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, find_piece_captured(to_index)));
            moves.push_back(Move(to_index-9, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, find_piece_captured(to_index)));
            moves.push_back(Move(to_index-9, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, find_piece_captured(to_index)));
        } while (east_promotion_attacks &= east_promotion_attacks - 1);
        
        if (west_regular_attacks) do {
            int to_index = bitscan_forward(west_regular_attacks);
            moves.push_back(Move(to_index-7, to_index, MOVE_NORMAL, 0, 6, find_piece_captured(to_index)));
        } while (west_regular_attacks &= west_regular_attacks - 1);
        
        if (west_promotion_attacks) do {
            int to_index = bitscan_forward(west_promotion_attacks);
            moves.push_back(Move(to_index-7, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, find_piece_captured(to_index)));
            moves.push_back(Move(to_index-7, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, find_piece_captured(to_index)));
            moves.push_back(Move(to_index-7, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, find_piece_captured(to_index)));
            moves.push_back(Move(to_index-7, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, find_piece_captured(to_index)));
        } while (west_promotion_attacks &= west_promotion_attacks - 1);
        
        
        // Quiet moves:
        U64 pawn_regular_pushes = ((pawns << 8) & ~eighth_rank) & ~(occ);
        pawn_regular_pushes &= block_check_masks;
        U64 pawn_double_pushes = ((pawn_regular_pushes & (first_rank << 16)) << 8) & ~(occ);
        pawn_double_pushes &= block_check_masks;
        U64 pawn_promotion_pushes = ((pawns << 8) & eighth_rank) & ~(occ);
        pawn_promotion_pushes &= block_check_masks;
        
        
        if (pawn_regular_pushes) do {
            int to_index = bitscan_forward(pawn_regular_pushes);
            moves.push_back(Move(to_index-8, to_index, MOVE_NORMAL, 0, PIECE_PAWN, 0));
        } while (pawn_regular_pushes &= pawn_regular_pushes - 1);
        
        if (pawn_double_pushes) do {
            int to_index = bitscan_forward(pawn_double_pushes);
            moves.push_back(Move(to_index-16, to_index, MOVE_NORMAL, 0, PIECE_PAWN, 0));
        } while (pawn_double_pushes &= pawn_double_pushes - 1);
        
        if (pawn_promotion_pushes) do {
            int to_index = bitscan_forward(pawn_promotion_pushes);
            moves.push_back(Move(to_index-8, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, 0));
            moves.push_back(Move(to_index-8, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, 0));
            moves.push_back(Move(to_index-8, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, 0));
            moves.push_back(Move(to_index-8, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, 0));
        } while (pawn_promotion_pushes &= pawn_promotion_pushes - 1);
    }
}

void Board::generate_pawn_movesB(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces) {
    U64 pawns = Bitboards[Pawns] & friendly_pieces;
    
    if (pawns) {
        // East attacks:
        U64 east_attacks = ((pawns >> 7) & ~a_file) & Bitboards[WhitePieces];
        east_attacks &= block_check_masks;
        // filter out promotions
        U64 east_promotion_attacks = east_attacks & first_rank;
        east_promotion_attacks &= block_check_masks;
        U64 east_regular_attacks = east_attacks & ~first_rank;
        east_promotion_attacks &= block_check_masks;
        
        
        // West attacks:
        U64 west_attacks = ((pawns >> 9) & ~h_file) & Bitboards[WhitePieces];
        west_attacks &= block_check_masks;
        U64 west_promotion_attacks = west_attacks & first_rank;
        west_promotion_attacks &= block_check_masks;
        U64 west_regular_attacks = west_attacks & ~first_rank;
        west_regular_attacks &= block_check_masks;
        

        
        // Serialize into moves:
        if (east_regular_attacks) do {
            int to_index = bitscan_forward(east_regular_attacks);
            moves.push_back(Move(to_index+7, to_index, MOVE_NORMAL, 0, PIECE_PAWN, find_piece_captured(to_index)));
        } while (east_regular_attacks &= east_regular_attacks - 1);
        
        if (east_promotion_attacks) do {
            int to_index = bitscan_forward(east_promotion_attacks);
            moves.push_back(Move(to_index+7, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, find_piece_captured(to_index)));
            moves.push_back(Move(to_index+7, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, find_piece_captured(to_index)));
            moves.push_back(Move(to_index+7, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, find_piece_captured(to_index)));
            moves.push_back(Move(to_index+7, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, find_piece_captured(to_index)));
        } while (east_promotion_attacks &= east_promotion_attacks - 1);
        
        if (west_regular_attacks) do {
            int to_index = bitscan_forward(west_regular_attacks);
            moves.push_back(Move(to_index+9, to_index, MOVE_NORMAL, 0, PIECE_PAWN, find_piece_captured(to_index)));
        } while (west_regular_attacks &= west_regular_attacks - 1);
        
        if (west_promotion_attacks) do {
            int to_index = bitscan_forward(west_promotion_attacks);
            moves.push_back(Move(to_index+9, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, find_piece_captured(to_index)));
            moves.push_back(Move(to_index+9, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, find_piece_captured(to_index)));
            moves.push_back(Move(to_index+9, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, find_piece_captured(to_index)));
            moves.push_back(Move(to_index+9, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, find_piece_captured(to_index)));
        } while (west_promotion_attacks &= west_promotion_attacks - 1);
        
        
        // Quiet moves:
        U64 pawn_regular_pushes = ((pawns >> 8) & ~first_rank) & ~(occ);
        pawn_regular_pushes &= block_check_masks;
        U64 pawn_double_pushes = ((pawn_regular_pushes & (eighth_rank >> 16)) >> 8) & ~(occ);
        pawn_double_pushes &= block_check_masks;
        U64 pawn_promotion_pushes = ((pawns >> 8) & first_rank) & ~(occ);
        pawn_promotion_pushes &= block_check_masks;
        
        
        if (pawn_regular_pushes) do {
            int to_index = bitscan_forward(pawn_regular_pushes);
            moves.push_back(Move(to_index+8, to_index, MOVE_NORMAL, 0, PIECE_PAWN, 0));
        } while (pawn_regular_pushes &= pawn_regular_pushes - 1);
        
        if (pawn_double_pushes) do {
            int to_index = bitscan_forward(pawn_double_pushes);
            moves.push_back(Move(to_index+16, to_index, MOVE_NORMAL, 0, PIECE_PAWN, 0));
        } while (pawn_double_pushes &= pawn_double_pushes - 1);
        
        if (pawn_promotion_pushes) do {
            int to_index = bitscan_forward(pawn_promotion_pushes);
            moves.push_back(Move(to_index+8, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, 0));
            moves.push_back(Move(to_index+8, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, 0));
            moves.push_back(Move(to_index+8, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, 0));
            moves.push_back(Move(to_index+8, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, 0));
        } while (pawn_promotion_pushes &= pawn_promotion_pushes - 1);
    }
}



void Board::generate_knight_moves(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces) {
    U64 knights = Bitboards[Knights] & friendly_pieces;
    
    if (knights) do {
        int from_index = bitscan_forward(knights);
        U64 move_targets = knight_paths[from_index] & ~friendly_pieces;
        move_targets &= block_check_masks;
        if (move_targets) do {
            int to_index = bitscan_forward(move_targets);
            moves.push_back(Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_KNIGHT, find_piece_captured(to_index)));
        } while (move_targets &= move_targets - 1);
        
    } while (knights &= knights - 1);
}


void Board::generate_bishop_moves(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces) {
    U64 bishops = Bitboards[Bishops] & friendly_pieces;
    
    if (bishops) do {
        int from_index = bitscan_forward(bishops);
        U64 move_targets = bishop_attacks(from_index, occ);
        move_targets &= ~friendly_pieces;
        move_targets &= block_check_masks;
        
        if (move_targets) do {
            int to_index = bitscan_forward(move_targets);
            moves.push_back(Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_BISHOP, find_piece_captured(to_index)));
        } while (move_targets &= move_targets - 1);
        
    } while (bishops &= bishops - 1);
}

void Board::generate_rook_moves(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces) {
    U64 rooks = Bitboards[Rooks] & friendly_pieces;
    
    if (rooks) do {
        int from_index = bitscan_forward(rooks);
        U64 move_targets = rook_attacks(from_index, occ);
        move_targets &= ~friendly_pieces;
        move_targets &= block_check_masks;
        
        
        if (move_targets) do {
            int to_index = bitscan_forward(move_targets);
            moves.push_back(Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_ROOK, find_piece_captured(to_index)));
        } while (move_targets &= move_targets - 1);
        
    } while (rooks &= rooks - 1);
}

void Board::generate_queen_moves(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces) {
    U64 queens = Bitboards[Queens] & friendly_pieces;
    
    if (queens) do {
        int from_index = bitscan_forward(queens);
        U64 move_targets = bishop_attacks(from_index, occ) | rook_attacks(from_index, occ);
        move_targets &= ~friendly_pieces;
        move_targets &= block_check_masks;
        
        if (move_targets) do {
            int to_index = bitscan_forward(move_targets);
            moves.push_back(Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_QUEEN, find_piece_captured(to_index)));
        } while (move_targets &= move_targets - 1);
        
    } while (queens &= queens - 1);
}


// Legality section

U64 Board::attacks_to(int index, U64 occ) {
    // Important: this function does not factor in king attacks
    U64 enemy_pawns = Bitboards[Pawns] & Bitboards[!current_turn];
    U64 enemy_knights = Bitboards[Knights] & Bitboards[!current_turn];
    U64 enemy_bishops_queens = (Bitboards[Bishops] | Bitboards[Queens]) & Bitboards[!current_turn];
    U64 enemy_rooks_queens = (Bitboards[Rooks] | Bitboards[Queens]) & Bitboards[!current_turn];
    
    return (pawn_attacks[current_turn][index] & enemy_pawns)
         | (knight_paths[index] & enemy_knights)
         | (bishop_attacks(index, occ) & enemy_bishops_queens)
         | (rook_attacks(index, occ) & enemy_rooks_queens)
    ;
}

int Board::is_attacked(int index, U64 occ) {
    // Important: this function does not factor in king attacks
    U64 enemy_king = Bitboards[Kings] & Bitboards[!current_turn];
    U64 enemy_pawns = Bitboards[Pawns] & Bitboards[!current_turn];
    U64 enemy_knights = Bitboards[Knights] & Bitboards[!current_turn];
    U64 enemy_bishops_queens = (Bitboards[Bishops] | Bitboards[Queens]) & Bitboards[!current_turn];
    U64 enemy_rooks_queens = (Bitboards[Rooks] | Bitboards[Queens]) & Bitboards[!current_turn];
    
    return  (pawn_attacks[current_turn][index] & enemy_pawns)
         || (knight_paths[index] & enemy_knights)
         || (bishop_attacks(index, occ) & enemy_bishops_queens)
         || (rook_attacks(index, occ) & enemy_rooks_queens)
         || (king_paths[index] & enemy_king)
    ;
}


U64 Board::calculate_block_masks(U64 king_attacker) {
    U64 block_mask = 0;
    U64 capture_mask = king_attacker;
    
    U64 slider_attacker = king_attacker & ~(Bitboards[Pawns] | Bitboards[Knights]);
    if (slider_attacker) {
        int attacker_index = bitscan_forward(slider_attacker);
        int king_index = bitscan_forward(Bitboards[Kings] & Bitboards[current_turn]);
        
        block_mask = in_between_mask(king_index, attacker_index);
    }
    return block_mask | capture_mask;
}

U64 Board::calculate_bishop_pins(int* pinners, U64 occ, U64 friendly_pieces) {
    int king_index = bitscan_forward(Bitboards[Kings] & Bitboards[current_turn]);
}

U64 Board::calculate_rook_pins(int* pinners, U64 occ, U64 friendly_pieces) {
    
}


// MOVE GENERATION END
