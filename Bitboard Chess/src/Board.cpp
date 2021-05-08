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
    standard_setup();
}

Board::Board(std::string str) {
    read_FEN(str);
    standard_setup();
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

void Board::standard_setup() {
    calculate_piece_values();
}

bool Board::get_current_turn() {
    return current_turn;
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
    return PIECE_QUEEN * ((Bitboards[Queens] & bit) != 0) + PIECE_ROOK * ((Bitboards[Rooks] & bit) != 0) + PIECE_BISHOP * ((Bitboards[Bishops] & bit) != 0) + PIECE_KNIGHT * ((Bitboards[Knights] & bit) != 0) + PIECE_PAWN * ((Bitboards[Pawns] & bit) != 0);
    */

    U64 bit = C64(1) << index;
    if (!(Bitboards[!current_turn] & bit)) {
        return PIECE_NONE;
    }
    else if (Bitboards[Pawns] & bit) {
        return PIECE_PAWN;
    }
    else if (Bitboards[Knights] & bit) {
        return PIECE_KNIGHT;
    }
    else if (Bitboards[Bishops] & bit) {
        return PIECE_BISHOP;
    }
    else if (Bitboards[Rooks] & bit) {
        return PIECE_ROOK;
    }
    else if (Bitboards[Queens] & bit) {
        return PIECE_QUEEN;
    }
    else {
        return PIECE_NONE;
    }
}

unsigned int Board::find_piece_captured_without_occ(int index) {
    // This function does not check if the capture is illegal, make sure to &~friendly_pieces beforehand.
    // Also, it doesn't check king captures
    
    /*
    U64 bit = C64(1) << index;
    return 2 * ((Bitboards[Queens] & bit) != 0) + 3 * ((Bitboards[Rooks] & bit) != 0) + 4 * ((Bitboards[Bishops] & bit) != 0) + 5 * ((Bitboards[Knights] & bit) != 0) + 6 * ((Bitboards[Pawns] & bit) != 0);
     */

    U64 bit = C64(1) << index;
    if (Bitboards[Pawns] & bit) {
        return PIECE_PAWN;
    }
    else if (Bitboards[Knights] & bit) {
        return PIECE_KNIGHT;
    }
    else if (Bitboards[Bishops] & bit) {
        return PIECE_BISHOP;
    }
    else if (Bitboards[Rooks] & bit) {
        return PIECE_ROOK;
    }
    else if (Bitboards[Queens] & bit) {
        return PIECE_QUEEN;
    }
    else {
        return PIECE_NONE;
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

U64 Board::xray_bishop_attacks(int from_index, U64 occ, U64 blockers) {
    U64 attacks = bishop_attacks(from_index, occ);
    blockers &= attacks;
    return attacks ^ bishop_attacks(from_index, occ ^ blockers);
}

U64 Board::xray_rook_attacks(int from_index, U64 occ, U64 blockers) {
    U64 attacks = rook_attacks(from_index, occ);
    blockers &= attacks;
    return attacks ^ rook_attacks(from_index, occ ^ blockers);
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
    U64 block_masks = UniverseBoard;
    
    int pinners[8];
    
    U64 occ = Bitboards[WhitePieces] | Bitboards[BlackPieces];
    U64 friendly_pieces = Bitboards[current_turn];
    int king_index = bitscan_forward(Bitboards[Kings] & friendly_pieces);
    
    king_attackers = attacks_to(bitscan_forward(Bitboards[Kings] & friendly_pieces), occ);
    num_attackers = _mm_popcnt_u64(king_attackers);
    
    king_is_in_check = num_attackers >= 1;
    
    generate_king_moves(moves, occ, friendly_pieces, king_index, num_attackers);
    
    if (num_attackers == 1) {
        block_masks = calculate_block_masks(king_attackers);
    }
    else if (num_attackers > 1) {
        return;
    }
    
    
    bishop_pinned = calculate_bishop_pins(pinners, occ, friendly_pieces);
    rook_pinned = calculate_rook_pins(pinners, occ, friendly_pieces);

    if (current_turn == 0) {
        generate_pawn_movesW(moves, block_masks, occ, friendly_pieces, pinners, rook_pinned, bishop_pinned, king_index);
    }
    else {
        generate_pawn_movesB(moves, block_masks, occ, friendly_pieces, pinners, rook_pinned, bishop_pinned, king_index);
    }
    generate_knight_moves(moves, block_masks, occ, friendly_pieces, rook_pinned, bishop_pinned);
    generate_bishop_moves(moves, block_masks, occ, friendly_pieces, pinners, rook_pinned, bishop_pinned, king_index);
    generate_rook_moves(moves, block_masks, occ, friendly_pieces, pinners, rook_pinned, bishop_pinned, king_index);
    generate_queen_moves(moves, block_masks, occ, friendly_pieces, pinners, rook_pinned, bishop_pinned, king_index);
}


void Board::generate_king_moves(std::vector<Move>& moves, U64 occ, U64 friendly_pieces, int king_index, int num_attackers) {

    U64 move_targets = king_paths[king_index] & ~friendly_pieces;
    
    if (current_turn == 0) {
        if (white_can_castle_queenside && !num_attackers && !(C64(0xE) & occ) && !is_attacked(3, occ) && !is_attacked(2, occ)) {
            moves.push_back(Move(4, 2, MOVE_CASTLING, CASTLE_TYPE_QUEENSIDE, PIECE_KING, PIECE_NONE));
        }
    
        if (white_can_castle_kingside && !num_attackers && !(C64(0x60) & occ) && !is_attacked(5, occ) && !is_attacked(6, occ)) {
            moves.push_back(Move(4, 6, MOVE_CASTLING, CASTLE_TYPE_KINGSIDE, PIECE_KING, PIECE_NONE));
        }
    }
    else {
        if (black_can_castle_queenside && !num_attackers && !(C64(0xE00000000000000) & occ) && !is_attacked(59, occ) && !is_attacked(58, occ)) {
            moves.push_back(Move(60, 58, MOVE_CASTLING, CASTLE_TYPE_QUEENSIDE, PIECE_KING, PIECE_NONE));
        }
    
        if (black_can_castle_kingside && !num_attackers && !(C64(0x6000000000000000) & occ) && !is_attacked(61, occ) && !is_attacked(62, occ)) {
            moves.push_back(Move(60, 62, MOVE_CASTLING, CASTLE_TYPE_KINGSIDE, PIECE_KING, PIECE_NONE));
        }
    }

    U64 occ_without_friendly_king = occ ^ (C64(1) << king_index);
    
    if (move_targets) do {
        int to_index = bitscan_forward(move_targets);
        if (!is_attacked(to_index, occ_without_friendly_king)) {
            moves.push_back(Move(king_index, to_index, MOVE_NORMAL, 0, PIECE_KING, find_piece_captured(to_index)));
        }
    } while (move_targets &= move_targets - 1);
}



void Board::generate_pawn_movesW(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners, U64 rook_pinned, U64 bishop_pinned, int king_index) {
    
    U64 pawns = Bitboards[Pawns] & friendly_pieces & ~rook_pinned & ~bishop_pinned;
    
    if (Bitboards[Pawns] & friendly_pieces & ~bishop_pinned) {
        // East attacks:
        U64 east_attacks = ((pawns << 9) & ~a_file) & Bitboards[BlackPieces];
        east_attacks &= block_check_masks;
        // filter out promotions
        U64 east_promotion_attacks = east_attacks & eighth_rank;
        U64 east_regular_attacks = east_attacks & ~eighth_rank;
        
        
        // West attacks:
        U64 west_attacks = ((pawns << 7) & ~h_file) & Bitboards[BlackPieces];
        west_attacks &= block_check_masks;
        U64 west_promotion_attacks = west_attacks & eighth_rank;
        U64 west_regular_attacks = west_attacks & ~eighth_rank;
        
        
        
        // Serialize into moves:
        if (east_regular_attacks) do {
            int to_index = bitscan_forward(east_regular_attacks);
            moves.push_back(Move(to_index-9, to_index, MOVE_NORMAL, 0, PIECE_PAWN, find_piece_captured_without_occ(to_index)));
        } while (east_regular_attacks &= east_regular_attacks - 1);
        
        if (east_promotion_attacks) do {
            int to_index = bitscan_forward(east_promotion_attacks);
            auto piece_captured = find_piece_captured_without_occ(to_index);
            moves.push_back(Move(to_index-9, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
            moves.push_back(Move(to_index-9, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, piece_captured));
            moves.push_back(Move(to_index-9, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, piece_captured));
            moves.push_back(Move(to_index-9, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, piece_captured));
        } while (east_promotion_attacks &= east_promotion_attacks - 1);
        
        if (west_regular_attacks) do {
            int to_index = bitscan_forward(west_regular_attacks);
            moves.push_back(Move(to_index-7, to_index, MOVE_NORMAL, 0, PIECE_PAWN, find_piece_captured_without_occ(to_index)));
        } while (west_regular_attacks &= west_regular_attacks - 1);
        
        if (west_promotion_attacks) do {
            int to_index = bitscan_forward(west_promotion_attacks);
            auto piece_captured = find_piece_captured_without_occ(to_index);
            moves.push_back(Move(to_index-7, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
            moves.push_back(Move(to_index-7, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, piece_captured));
            moves.push_back(Move(to_index-7, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, piece_captured));
            moves.push_back(Move(to_index-7, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, piece_captured));
        } while (west_promotion_attacks &= west_promotion_attacks - 1);
        
        
        // Quiet moves:
        
        // Add Northern rook pins (only type of pin that pawn_push can move in)
        U64 north_and_south_of_king = rays[North][king_index] | rays[South][king_index];
        
        U64 pawn_regular_pushes = (((pawns | (north_and_south_of_king & Bitboards[Pawns] & rook_pinned)) << 8) & ~eighth_rank) & ~(occ);
        U64 pawn_double_pushes = ((pawn_regular_pushes & (first_rank << 16)) << 8) & ~(occ);
                
        pawn_regular_pushes &= block_check_masks;
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
    
    // Pinned pawns are handled individually:
    U64 pinned_pawn_attacks = Bitboards[Pawns] & bishop_pinned; // No rook pinned since attacks can't happen when pinned by rook
    
    if (pinned_pawn_attacks) do {
        int from_index = bitscan_forward(pinned_pawn_attacks);
        U64 move_targets = pawn_attacks[WhitePieces][from_index];
        move_targets &= block_check_masks;
        move_targets &= C64(1) << *(pinners + direction_between[king_index][from_index]);
        
        if (move_targets) {
            int to_index = bitscan_forward(move_targets);
            auto piece_captured = find_piece_captured_without_occ(to_index);
            if (from_index >= 48) {
                moves.push_back(Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
                moves.push_back(Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, piece_captured));
                moves.push_back(Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
                moves.push_back(Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
            }
            else {
                moves.push_back(Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_PAWN, piece_captured));
            }
        }
        
    } while (pinned_pawn_attacks &= pinned_pawn_attacks - 1);
    
    // En Passant:
    if (en_passant_square != -1 && (((C64(1) << en_passant_square) & block_check_masks) || (C64(1) << (en_passant_square - 8) & block_check_masks))) {
        U64 en_passant_pawn_source = pawns & pawn_attacks[BlackPieces][en_passant_square];
        
    
        if (en_passant_pawn_source) do {
            int from_index = bitscan_forward(en_passant_pawn_source);
            
            // Find if en_passant square has the possibility of being pinned:
            if ((C64(1) << from_index) & (rays[East][king_index] | rays[West][king_index])) {
                U64 occ_minus_ep_pawns = occ ^ ((C64(1) << (en_passant_square - 8)) | (C64(1) << from_index));
                if ((get_negative_ray_attacks(king_index, West, occ_minus_ep_pawns) | get_positive_ray_attacks(king_index, East, occ_minus_ep_pawns)) & (Bitboards[Rooks] | Bitboards[Queens]) & Bitboards[!current_turn]) {
                    continue;
                }
            }

            moves.push_back(Move(from_index, en_passant_square, MOVE_ENPASSANT, 0, PIECE_PAWN, PIECE_PAWN));
        } while (en_passant_pawn_source &= en_passant_pawn_source - 1);
    
        // En Passant pins:
        U64 positive_diag_rays_from_king = rays[NorthWest][king_index] | rays[NorthEast][king_index];
        U64 en_passants_along_pin_path = positive_diag_rays_from_king & (C64(1) << en_passant_square);
        U64 pinned_en_passant = Bitboards[Pawns] & bishop_pinned & pawn_attacks[BlackPieces][en_passant_square];
        pinned_en_passant &= block_check_masks;

        if (pinned_en_passant && en_passants_along_pin_path) {
            moves.push_back(Move(bitscan_forward(pinned_en_passant), en_passant_square, MOVE_ENPASSANT, 0, PIECE_PAWN, PIECE_PAWN));
        }
    }
}

void Board::generate_pawn_movesB(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners, U64 rook_pinned, U64 bishop_pinned, int king_index) {
    U64 pawns = Bitboards[Pawns] & friendly_pieces & ~rook_pinned & ~bishop_pinned;
    
    if (Bitboards[Pawns] & friendly_pieces & ~bishop_pinned) {
        // East attacks:
        U64 east_attacks = ((pawns >> 7) & ~a_file) & Bitboards[WhitePieces];
        east_attacks &= block_check_masks;
        // filter out promotions
        U64 east_promotion_attacks = east_attacks & first_rank;
        U64 east_regular_attacks = east_attacks & ~first_rank;
        
        
        // West attacks:
        U64 west_attacks = ((pawns >> 9) & ~h_file) & Bitboards[WhitePieces];
        west_attacks &= block_check_masks;
        U64 west_promotion_attacks = west_attacks & first_rank;
        U64 west_regular_attacks = west_attacks & ~first_rank;
        

        
        // Serialize into moves:
        if (east_regular_attacks) do {
            int to_index = bitscan_forward(east_regular_attacks);
            moves.push_back(Move(to_index+7, to_index, MOVE_NORMAL, 0, PIECE_PAWN, find_piece_captured_without_occ(to_index)));
        } while (east_regular_attacks &= east_regular_attacks - 1);
        
        if (east_promotion_attacks) do {
            int to_index = bitscan_forward(east_promotion_attacks);
            auto piece_captured = find_piece_captured_without_occ(to_index);
            moves.push_back(Move(to_index+7, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
            moves.push_back(Move(to_index+7, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, piece_captured));
            moves.push_back(Move(to_index+7, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, piece_captured));
            moves.push_back(Move(to_index+7, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, piece_captured));
        } while (east_promotion_attacks &= east_promotion_attacks - 1);
        
        if (west_regular_attacks) do {
            int to_index = bitscan_forward(west_regular_attacks);
            moves.push_back(Move(to_index+9, to_index, MOVE_NORMAL, 0, PIECE_PAWN, find_piece_captured_without_occ(to_index)));
        } while (west_regular_attacks &= west_regular_attacks - 1);
        
        if (west_promotion_attacks) do {
            int to_index = bitscan_forward(west_promotion_attacks);
            auto piece_captured = find_piece_captured_without_occ(to_index);
            moves.push_back(Move(to_index+9, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
            moves.push_back(Move(to_index+9, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, piece_captured));
            moves.push_back(Move(to_index+9, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, piece_captured));
            moves.push_back(Move(to_index+9, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, piece_captured));
        } while (west_promotion_attacks &= west_promotion_attacks - 1);
        
        
        // Quiet moves:
        
        // Add Southern rook pins (only type of pin that pawn_push can move in)
        U64 north_and_south_of_king = rays[North][king_index] | rays[South][king_index];
        
        U64 pawn_regular_pushes = (((pawns | (north_and_south_of_king & Bitboards[Pawns] & rook_pinned)) >> 8) & ~first_rank) & ~(occ);
        U64 pawn_double_pushes = ((pawn_regular_pushes & (eighth_rank >> 16)) >> 8) & ~(occ);
        
        pawn_regular_pushes &= block_check_masks;
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
    
    // Pinned pawns are handled individually:
    U64 pinned_pawn_attacks = Bitboards[Pawns] & bishop_pinned; // No rook pinned since attacks can't happen when pinned by rook
    
    if (pinned_pawn_attacks) do {
        int from_index = bitscan_forward(pinned_pawn_attacks);
        U64 move_targets = pawn_attacks[BlackPieces][from_index];
        move_targets &= block_check_masks;
        move_targets &= C64(1) << *(pinners + direction_between[king_index][from_index]);
        
        if (move_targets) {
            int to_index = bitscan_forward(move_targets);
            auto piece_captured = find_piece_captured_without_occ(to_index);
            if (from_index <= 7) {
                moves.push_back(Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
                moves.push_back(Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, piece_captured));
                moves.push_back(Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
                moves.push_back(Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
            }
            else {
                moves.push_back(Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_PAWN, piece_captured));
            }
        }
        
    } while (pinned_pawn_attacks &= pinned_pawn_attacks - 1);
    
    // En Passant:
    if (en_passant_square != -1 && (((C64(1) << en_passant_square) & block_check_masks) || (C64(1) << (en_passant_square + 8) & block_check_masks))) {
        U64 en_passant_pawn_source = pawns & pawn_attacks[WhitePieces][en_passant_square];
    
        if (en_passant_pawn_source) do {
            int from_index = bitscan_forward(en_passant_pawn_source);
            
            // Find if en_passant square has the possibility of being pinned:
            if ((C64(1) << from_index) & (rays[East][king_index] | rays[West][king_index])) {
                U64 occ_minus_ep_pawns = occ ^ ((C64(1) << (en_passant_square + 8)) | (C64(1) << from_index));
                if ((get_negative_ray_attacks(king_index, West, occ_minus_ep_pawns) | get_positive_ray_attacks(king_index, East, occ_minus_ep_pawns)) & (Bitboards[Rooks] | Bitboards[Queens]) & Bitboards[!current_turn]) {
                    continue;
                }
            }
            
            moves.push_back(Move(from_index, en_passant_square, MOVE_ENPASSANT, 0, PIECE_PAWN, PIECE_PAWN));
        } while (en_passant_pawn_source &= en_passant_pawn_source - 1);
    
        // En Passant pins:
        U64 negative_diag_rays_from_king = rays[SouthWest][king_index] | rays[SouthEast][king_index];
        U64 en_passants_along_pin_path = negative_diag_rays_from_king & (C64(1) << en_passant_square);
        U64 pinned_en_passant = Bitboards[Pawns] & bishop_pinned & pawn_attacks[WhitePieces][en_passant_square];
        pinned_en_passant &= block_check_masks;

        if (pinned_en_passant && en_passants_along_pin_path) {
            moves.push_back(Move(bitscan_forward(pinned_en_passant), en_passant_square, MOVE_ENPASSANT, 0, PIECE_PAWN, PIECE_PAWN));
        }
    }
}



void Board::generate_knight_moves(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, U64 rook_pinned, U64 bishop_pinned) {
    U64 knights = Bitboards[Knights] & friendly_pieces & ~rook_pinned & ~bishop_pinned; // Knights can't move at all when pinned
    
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


void Board::generate_bishop_moves(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners, U64 rook_pinned, U64 bishop_pinned, int king_index) {
    U64 bishops = Bitboards[Bishops] & friendly_pieces & ~rook_pinned & ~bishop_pinned; // Bishops can't move when pinned by a rook
    U64 bishops_bishop_pinned = Bitboards[Bishops] & bishop_pinned;
    
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
    
    if (bishops_bishop_pinned) do {
        int from_index = bitscan_forward(bishops_bishop_pinned);
        U64 move_targets = bishop_attacks(from_index, occ);
        move_targets &= block_check_masks;
        move_targets &= in_between_mask(king_index, *(pinners + direction_between[king_index][from_index]));

        if (move_targets) do {
            int to_index = bitscan_forward(move_targets);
            moves.push_back(Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_BISHOP, find_piece_captured(to_index)));
        } while (move_targets &= move_targets - 1);
        
    } while (bishops_bishop_pinned &= bishops_bishop_pinned - 1);
}

void Board::generate_rook_moves(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners, U64 rook_pinned, U64 bishop_pinned, int king_index) {
    U64 rooks = Bitboards[Rooks] & friendly_pieces & ~rook_pinned & ~bishop_pinned;
    U64 rooks_rook_pinned = Bitboards[Rooks] & rook_pinned;
    
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
    
    if (rooks_rook_pinned) do {
        int from_index = bitscan_forward(rooks_rook_pinned);
        U64 move_targets = rook_attacks(from_index, occ);
        move_targets &= block_check_masks;
        move_targets &= in_between_mask(king_index, *(pinners + direction_between[king_index][from_index]));

        if (move_targets) do {
            int to_index = bitscan_forward(move_targets);
            moves.push_back(Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_ROOK, find_piece_captured(to_index)));
        } while (move_targets &= move_targets - 1);
        
    } while (rooks_rook_pinned &= rooks_rook_pinned - 1);
}

void Board::generate_queen_moves(std::vector<Move>& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners, U64 rook_pinned, U64 bishop_pinned, int king_index) {
    U64 queens = Bitboards[Queens] & friendly_pieces & ~rook_pinned & ~bishop_pinned;
    U64 queens_pinned = Bitboards[Queens] & (rook_pinned | bishop_pinned);
    
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
    
    if (queens_pinned) do {
        int from_index = bitscan_forward(queens_pinned);
        U64 move_targets = bishop_attacks(from_index, occ) | rook_attacks(from_index, occ);
        move_targets &= block_check_masks;
        move_targets &= in_between_mask(king_index, *(pinners + direction_between[king_index][from_index]));

        if (move_targets) do {
            int to_index = bitscan_forward(move_targets);
            moves.push_back(Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_QUEEN, find_piece_captured(to_index)));
        } while (move_targets &= move_targets - 1);
        
    } while (queens_pinned &= queens_pinned - 1);
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
    // Important: this function factors in king attacks
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
    // Make sure to pass in int arr[8], otherwise segfault
    int king_index = bitscan_forward(Bitboards[Kings] & friendly_pieces);
    U64 pinner = xray_bishop_attacks(king_index, occ, friendly_pieces) & (Bitboards[Bishops] | Bitboards[Queens]) & Bitboards[!current_turn];
    U64 pinned = 0;
    while (pinner) {
        int sq  = bitscan_forward(pinner);
        // Save the index of pinner so we don't have to recalculate later:
        *(pinners + direction_between[king_index][sq]) = sq;
        
        pinned |= in_between_mask(king_index, sq) & friendly_pieces;
        pinner &= pinner - 1;
    }
    return pinned;
}

U64 Board::calculate_rook_pins(int* pinners, U64 occ, U64 friendly_pieces) {
    // Make sure to pass in int arr[8], otherwise segfault
    int king_index = bitscan_forward(Bitboards[Kings] & friendly_pieces);
    U64 pinner = xray_rook_attacks(king_index, occ, friendly_pieces) & (Bitboards[Rooks] | Bitboards[Queens]) & Bitboards[!current_turn];
    U64 pinned = 0;
    while (pinner) {
        int sq  = bitscan_forward(pinner);
        // Save the index of pinner so we don't have to recalculate later:
        *(pinners + direction_between[king_index][sq]) = sq;
        
        pinned |= in_between_mask(king_index, sq) & friendly_pieces;
        pinner &= pinner - 1;
    }
    return pinned;
}


void Board::make_move(Move move) {
    move_data move_data = {move, white_can_castle_queenside, white_can_castle_kingside, black_can_castle_queenside, black_can_castle_kingside, en_passant_square};
    move_stack.push_back(move_data);
    
    // Actual act of making move:
    int move_from_index = move.get_from();
    int move_to_index = move.get_to();
    U64 from_bb = C64(1) << move_from_index;
    U64 to_bb = C64(1) << move_to_index;
    

    
    // Set out the captured squares first
    if (move.get_piece_captured() != PIECE_NONE && move.get_special_flag() != MOVE_ENPASSANT) {
        Bitboards[move.get_piece_captured()] ^= to_bb;
        Bitboards[!current_turn] ^= to_bb;
    }
    // Flip the occupacy of the from square and to square
    Bitboards[current_turn] ^= from_bb | to_bb;
    Bitboards[move.get_piece_moved()] ^= from_bb | to_bb;
    
    
    // Special move handling now:
    
    switch (move.get_special_flag()) {
        case MOVE_NORMAL: {
            break;
        }
        case MOVE_CASTLING: {
            U64 side_mask = (!current_turn * first_rank) | (current_turn * eighth_rank);
            U64 castle_bits = ((a_file | (a_file << 3)) * move.get_castle_type()) | ((h_file | a_file << 5) * !move.get_castle_type());
            castle_bits &= side_mask;
            Bitboards[current_turn] ^= castle_bits;
            Bitboards[Rooks] ^= castle_bits;
            break;
        }
        case MOVE_ENPASSANT: {
            U64 delete_square;
            if (current_turn == 0) {
                delete_square = C64(1) << (move_to_index - 8);
            }
            else {
                delete_square = C64(1) << (move_to_index + 8);
            }
            Bitboards[!current_turn] ^= delete_square;
            Bitboards[Pawns] ^= delete_square;
            break;
        }
        case MOVE_PROMOTION: {
            Bitboards[Pawns] ^= to_bb;
            Bitboards[move.get_promote_to() + 3] ^= to_bb;
            break;
        }
    }
    
    // Set en_passant square
    if (move.get_piece_moved() == PIECE_PAWN && abs(move_from_index - move_to_index) == 16) {
        if (current_turn == 0) {
            en_passant_square = move_to_index - 8;
        }
        else {
            en_passant_square = move_to_index + 8;
        }
    }
    else {
        en_passant_square = -1;
    }
    
    // Check to see if castling is invalidated

    // see if rook was captured/moved
    if (move.get_piece_captured() != PIECE_NONE) {
        switch (move_to_index) {
            case 56:
                black_can_castle_queenside = false;
                break;
            case 63:
                black_can_castle_kingside = false;
                break;
            case 0:
                white_can_castle_queenside = false;
                break;
            case 7:
                white_can_castle_kingside = false;
                break;
        }
    }
    switch (move_from_index) {
        case 56:
            black_can_castle_queenside = false;
            break;
        case 63:
            black_can_castle_kingside = false;
            break;
        case 0:
            white_can_castle_queenside = false;
            break;
        case 7:
            white_can_castle_kingside = false;
            break;
    }


    // Check if king moves
    if (move.get_piece_moved() == PIECE_KING) {
        if (current_turn == 1) {
            black_can_castle_kingside = false;
            black_can_castle_queenside = false;
        }
        else {
            white_can_castle_kingside = false;
            white_can_castle_queenside = false;
        }
    }
    
    // Switch turns:
    
    current_turn = !current_turn;
    
}

void Board::unmake_move() {
    
    current_turn = !current_turn;
    
    move_data last_move = move_stack.back();
    Move move = last_move.move;
    
    white_can_castle_queenside = last_move.white_can_castle_queenside;
    white_can_castle_kingside = last_move.white_can_castle_kingside;
    black_can_castle_queenside = last_move.black_can_castle_queenside;
    black_can_castle_kingside = last_move.black_can_castle_kingside;
    
    en_passant_square = last_move.en_passant_square;
    
    // Actual act of making move:
    int move_from_index = move.get_from();
    int move_to_index = move.get_to();
    U64 from_bb = C64(1) << move_from_index;
    U64 to_bb = C64(1) << move_to_index;
    
    
    // Set back the captured squares first
    if (move.get_piece_captured() != PIECE_NONE && move.get_special_flag() != MOVE_ENPASSANT) {
        
        Bitboards[move.get_piece_captured()] ^= to_bb;
        Bitboards[!current_turn] ^= to_bb;
    }
    

    
    // Flip the occupacy of the from square and to square
    Bitboards[current_turn] ^= from_bb | to_bb;
    Bitboards[move.get_piece_moved()] ^= from_bb | to_bb;

    
    // Special move handling now:
    
    switch (move.get_special_flag()) {
        case MOVE_NORMAL: {
            break;
        }
        case MOVE_CASTLING: {
            U64 side_mask = (!current_turn * first_rank) | (current_turn * eighth_rank);
            U64 castle_bits = ((a_file | (a_file << 3)) * move.get_castle_type()) | ((h_file | a_file << 5) * !move.get_castle_type());
            castle_bits &= side_mask;
            Bitboards[current_turn] ^= castle_bits;
            Bitboards[Rooks] ^= castle_bits;
            break;
        }
        case MOVE_ENPASSANT: {
            U64 delete_square;
            if (current_turn == 0) {
                delete_square = C64(1) << (move_to_index - 8);
            }
            else {
                delete_square = C64(1) << (move_to_index + 8);
            }
            Bitboards[!current_turn] ^= delete_square;
            Bitboards[Pawns] ^= delete_square;
            break;
        }
        case MOVE_PROMOTION: {
            Bitboards[Pawns] ^= to_bb;
            Bitboards[move.get_promote_to() + 3] ^= to_bb;
            break;
        }
    }
    
    move_stack.pop_back();
}

long Board::Perft(int depth /* assuming >= 1 */) {
    
    if (depth == 0) {
//        print_board();
        return 1;
    }
    
    long nodes = 0;
    int n_moves = 0;

    std::vector<Move> moves;
    moves.reserve(256);
    generate_moves(moves);
    n_moves = moves.size();


    for (auto it = moves.begin(); it != moves.end(); ++it) {
        make_move(*it);
        nodes += Perft(depth - 1);
        unmake_move();
    }
    return nodes;
}


// MOVE GENERATION END


// EVALUATION BEGIN

void Board::calculate_piece_values() {
    U64 white_pieces = Bitboards[WhitePieces];
    U64 black_pieces = Bitboards[BlackPieces];
    piece_values[WhitePieces] =  PAWN_VALUE * _mm_popcnt_u64(Bitboards[Pawns] & white_pieces)
                        + KNIGHT_VALUE * _mm_popcnt_u64(Bitboards[Knights] & white_pieces)
                        + BISHOP_VALUE * _mm_popcnt_u64(Bitboards[Bishops] & white_pieces)
                        + ROOK_VALUE * _mm_popcnt_u64(Bitboards[Rooks] & white_pieces)
                        + QUEEN_VALUE * _mm_popcnt_u64(Bitboards[Queens] & white_pieces)
    ;
    piece_values[BlackPieces] =  PAWN_VALUE * _mm_popcnt_u64(Bitboards[Pawns] & black_pieces)
                        + KNIGHT_VALUE * _mm_popcnt_u64(Bitboards[Knights] & black_pieces)
                        + BISHOP_VALUE * _mm_popcnt_u64(Bitboards[Bishops] & black_pieces)
                        + ROOK_VALUE * _mm_popcnt_u64(Bitboards[Rooks] & black_pieces)
                        + QUEEN_VALUE * _mm_popcnt_u64(Bitboards[Queens] & black_pieces)
    ;
}

void Board::print_piece_values() {
    std::cout << "\nWhite Piece Values: " << piece_values[WhitePieces];
    std::cout << "\nBlack Piece Values: " << piece_values[BlackPieces] << '\n';
}

int Board::static_eval() {
    int eval = 0;

    eval += piece_values[WhitePieces] - piece_values[BlackPieces];


    eval *= current_turn * -1 + !current_turn;
    return eval;
}

bool Board::is_king_in_check() {
    return king_is_in_check;
}
