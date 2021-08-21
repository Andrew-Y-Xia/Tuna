//
//  Board.cpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//
#include "Board.hpp"


template void Board::generate_moves<ALL_MOVES>(MoveList& moves, bool& is_in_check);

template void Board::generate_moves<CAPTURES_ONLY>(MoveList& moves, bool& is_in_check);

template int Board::calculate_mobility<ALL_MOVES>(bool& is_in_check);

template int Board::calculate_mobility<CAPTURES_ONLY>(bool& is_in_check);

template void Board::generate_moves<ALL_MOVES>(MoveList& moves);

template void Board::generate_moves<CAPTURES_ONLY>(MoveList& moves);

template int Board::calculate_mobility<ALL_MOVES>();

template int Board::calculate_mobility<CAPTURES_ONLY>();

Board::Board() {
    // Initializer if no starting FEN is given
    // Defaults normal starting position
    read_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    reg_starting_pos = true;
    standard_setup();
}

Board::Board(std::string str) {
    // Initializer
    read_FEN(str);
    if (str == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
        reg_starting_pos = true;
    } else {
        reg_starting_pos = false;
    }
    standard_setup();
}


void Board::read_FEN(std::string str) {
    // Takes in a FEN and sets up the board accordingly
    // Function is ugly; is there a c++ equiv. of python's 'string.split()' ?

    // First, clear all bitboards
    for (int i = WhitePieces; i <= Pawns; i++) {
        Bitboards[i] = EmptyBoard;
    }

    // state_flag is track which section of the FEN we are currently processing
    int state_flag = 0;

    // Holders for particular sections of the FEN
    std::string en_passant_square_str, halfmove_str, fullmove_str;

    // It's easier to work in cordinates and then covert to 0-63 index
    int x = 0;
    int y = 0;

    // Iterate through FEN string
    for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
        // If there's a space, we must be moving on to next section
        if (*it == ' ') {
            state_flag += 1;
        } else if (state_flag == 0) {
            // First section is the position of all the pieces

            // '/' indicates we're moving to the next row; therefore increment y and reset x
            if (*it == '/') {
                x = 0;
                y += 1;
            } else {
                // Numbers indicate we need to skip forward (in the x-dir.) by that much
                if (isdigit(*it)) {
                    int blanks = *it - '0';
                    x += blanks;
                }
                    // Otherwise it'll be a piece that we need to assign on the bitboards
                else {
                    if (isupper(*it)) {
                        // Set to white
                        Bitboards[WhitePieces] |= C64(1) << cords_to_index(x, y);
                    } else {
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
                            std::cout << "This should not have been reached. Invalid piece: " << (char) tolower(*it)
                                      << '\n';
                    }

                    x++;
                }
            }
        }
            // Check who's turn it is
        else if (state_flag == 1) {
            if (*it == 'w') {
                current_turn = WHITE;
            } else {
                current_turn = BLACK;
            }
        }
            // Check the castling rights
        else if (state_flag == 2) {
            if (*it == '-') {
                white_can_castle_queenside = false;
                white_can_castle_kingside = false;
                black_can_castle_queenside = false;
                black_can_castle_kingside = false;
            } else if (*it == 'K') {
                white_can_castle_kingside = true;
            } else if (*it == 'Q') {
                white_can_castle_queenside = true;
            } else if (*it == 'k') {
                black_can_castle_kingside = true;
            } else if (*it == 'q') {
                black_can_castle_queenside = true;
            }
        }
            // For the rest of the sections, add them to corresponding strings to process later
        else if (state_flag == 3) {
            en_passant_square_str.append(1, *it);
        } else if (state_flag == 4) {
            halfmove_str.append(1, *it);
        } else if (state_flag == 5) {
            fullmove_str.append(1, *it);
        } else {
            std::cout << "This state should not have been reached. ReadFEN Error occured." << '\n';
        }
    }

    // Process counter strings
    halfmove_counter = std::stoi(halfmove_str);
    fullmove_counter = std::stoi(fullmove_str);

    // Process En Passant string
    if (en_passant_square_str[0] != '-') {
        switch (en_passant_square_str[0]) {
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
        y = 8 - (en_passant_square_str[1] - '0');

        this->en_passant_square = cords_to_index(x, y);
    } else {
        this->en_passant_square = -1;
    }
}

Move Board::read_SAN(std::string str) {
    // Reads Standard Algebraic Notation
    // Checks for legality of move

    // Check if there's a check or checkmate marker
    // If there is, remove it
    if (str.back() == '#' || str.back() == '+') {
        str.pop_back();
    }

    Move move;
    unsigned long str_last_index = str.length() - 1;

    unsigned int piece_moved;
    int from_index, to_index;

    int pinners[8];

    int king_index = bitscan_forward(Bitboards[current_turn] & Bitboards[Kings]);
    U64 occ = Bitboards[WhitePieces] | Bitboards[BlackPieces];
    U64 bishop_pinned = calculate_bishop_pins(pinners, occ, Bitboards[current_turn]);
    U64 rook_pinned = calculate_rook_pins(pinners, occ, Bitboards[current_turn]);

    MoveList moves;
    generate_moves(moves);

    if (str[0] == 'O') {
        int addon = 0;
        if (current_turn == WHITE) {
            from_index = 4;
            addon = 0;
        } else {
            from_index = 60;
            addon = 56;
        }
        if (str == "O-O") {
            move = Move(from_index, 6 + addon, MOVE_CASTLING, CASTLE_TYPE_KINGSIDE, PIECE_KING, PIECE_NONE);
        } else {
            assert(str == "O-O-O");
            move = Move(from_index, 2 + addon, MOVE_CASTLING, CASTLE_TYPE_QUEENSIDE, PIECE_KING, PIECE_NONE);
        }
    }

        // Non-pawn move
    else if (isupper(str[0]) && isalpha(str[0])) {
        piece_moved = piece_char_to_piece(str[0]);
        to_index = txt_square_to_index(str.substr(str_last_index - 1, 2));

        U64 target;

        switch (piece_moved) {
            case PIECE_KING: {
                target = Bitboards[current_turn] & Bitboards[Kings];
                break;
            }
            case PIECE_QUEEN: {
                target = Bitboards[current_turn] & Bitboards[Queens] &
                         (bishop_attacks(to_index, occ) | rook_attacks(to_index, occ));

                U64 queens_pinned = target & (rook_pinned | bishop_pinned);
                if (queens_pinned)
                    do {
                        int queen_index = bitscan_forward(queens_pinned);
                        if (!((C64(1) << to_index) &
                              in_between_mask(king_index, *(pinners + direction_between[king_index][queen_index])))) {
                            target &= ~(C64(1) << queen_index);
                        }
                    } while (queens_pinned &= queens_pinned - 1);

                break;
            }
            case PIECE_ROOK: {
                target = Bitboards[current_turn] & Bitboards[Rooks] & rook_attacks(to_index, occ) & ~bishop_pinned;
                // Check rook pins
                U64 rooks_rook_pinned = target & rook_pinned;
                if (rooks_rook_pinned)
                    do {
                        int rook_index = bitscan_forward(rooks_rook_pinned);
                        if (!((C64(1) << to_index) &
                              in_between_mask(king_index, *(pinners + direction_between[king_index][rook_index])))) {
                            target &= ~(C64(1) << rook_index);
                        }
                    } while (rooks_rook_pinned &= rooks_rook_pinned - 1);
                break;
            }
            case PIECE_BISHOP: {
                target = Bitboards[current_turn] & Bitboards[Bishops] & bishop_attacks(to_index, occ) & ~rook_pinned;

                U64 bishops_bishop_pinned = target & bishop_pinned;
                if (bishops_bishop_pinned)
                    do {
                        int bishop_index = bitscan_forward(bishops_bishop_pinned);
                        if (!((C64(1) << to_index) &
                              in_between_mask(king_index, *(pinners + direction_between[king_index][bishop_index])))) {
                            target &= ~(C64(1) << bishop_index);
                        }
                    } while (bishops_bishop_pinned &= bishops_bishop_pinned - 1);
                break;
            }
            case PIECE_KNIGHT: {
                target = Bitboards[current_turn] & Bitboards[Knights] & knight_paths[to_index] & ~bishop_pinned &
                         ~rook_pinned;
                break;
            }
            default:
                target = C64(0);
                std::cout << "read_SAN should not have been reached";
        }

        assert(target);

        if (pop_count(target) >= 2) {
            // Ambiguous case; check if file, rank, or square is used
            int from_sqr_count = 7;
            if (str[str_last_index - 2] == 'x') {
                from_sqr_count--;
            }

            if (str.length() == from_sqr_count) {
                from_index = txt_square_to_index(str.substr(1, 2));
            } else {
                if (isdigit(str[1])) {
                    from_index = bitscan_forward((first_rank << (8 * ((str[1] - '0') - 1))) & target);
                } else {
                    from_index = bitscan_forward((a_file << char_to_num(str[1])) & target);
                }
            }
        } else {
            from_index = bitscan_forward(target);
        }

        move = Move(from_index, to_index, MOVE_NORMAL, 0, piece_moved, find_piece_captured(to_index));
    }

        // Pawn captures
    else if (str[1] == 'x') {
        unsigned int promotion_piece = 0;
        unsigned int move_type = MOVE_NORMAL;

        to_index = txt_square_to_index(str.substr(2, 2));

        U64 target = Bitboards[current_turn] & Bitboards[Pawns] & pawn_attacks[!current_turn][to_index];

        assert(target);

        if (isdigit(str[0])) {
            from_index = bitscan_forward((first_rank << (8 * ((str[0] - '0') - 1))) & target);
        } else {
            from_index = bitscan_forward((a_file << char_to_num(str[0])) & target);
        }

        unsigned int piece_captured = find_piece_captured(to_index);

        if (str[str_last_index - 1] == '=') {
            promotion_piece = piece_char_to_piece(str[str_last_index]) - 3;
            move_type = MOVE_PROMOTION;
        } else if (piece_captured == PIECE_NONE) {
            move_type = MOVE_ENPASSANT;
            piece_captured = PIECE_PAWN;
        }


        move = Move(from_index, to_index, move_type, promotion_piece, PIECE_PAWN, piece_captured);
    }

        // Pawn pushes
    else {
        unsigned int promotion_piece = 0;
        unsigned int move_type = MOVE_NORMAL;

        to_index = txt_square_to_index(str.substr(0, 2));

        U64 target;

        if (current_turn == WHITE) {
            target = get_negative_ray_attacks(to_index, South, occ);
            from_index = bitscan_forward(target);
        } else {
            target = get_positive_ray_attacks(to_index, North, occ);
            from_index = bitscan_reverse(target);
        }


        if (str[str_last_index - 1] == '=') {
            promotion_piece = piece_char_to_piece(str[str_last_index]) - 3;
            move_type = MOVE_PROMOTION;
        }

        move = Move(from_index, to_index, move_type, promotion_piece, PIECE_PAWN, PIECE_NONE);
    }

    assert(moves.contains(move));
    return move;
}

Move Board::read_LAN(std::string str) {
    MoveList moves;
    generate_moves(moves);

    Move move;
    move.set_from(txt_square_to_index(str.substr(0, 2)));
    move.set_to(txt_square_to_index(str.substr(2, 2)));

    for (auto it = moves.begin(); it != moves.end(); it++) {
        if (it->first_twelfth_eq(move)) {
            move = *it;
            if (move.get_special_flag() == MOVE_PROMOTION) {
                switch (str[4]) {
                    case 'n':
                        move.set_promote_to(PROMOTE_TO_KNIGHT);
                        break;
                    case 'b':
                        move.set_promote_to(PROMOTE_TO_BISHOP);
                        break;
                    case 'r':
                        move.set_promote_to(PROMOTE_TO_ROOK);
                        break;
                    case 'q':
                        move.set_promote_to(PROMOTE_TO_QUEEN);
                        break;
                }
            }
            return move;
        }
    }
    Move i_move;
    i_move.set_as_illegal();
    return move;
}

void Board::standard_setup() {
//     Called during initialization
    calculate_piece_values();
    calculate_piece_square_values();
//    calculate_piece_count();
    hash();
}

void Board::hash() {
    // Hashes current position into a U64
    // Stores hash in z_key
    // ONLY CALL AFTER 'init_zorbist_bitstrings()' has been called

    // Clear hash
    z_key = C64(0);

    // Apply xor for turn
    if (current_turn == BLACK) {
        z_key ^= black_to_move_bitstring;
    }

    // Apply xor for castling rights
    if (white_can_castle_queenside) {
        z_key ^= white_castle_queenside_bitstring;
    }
    if (white_can_castle_kingside) {
        z_key ^= white_castle_kingside_bitstring;
    }
    if (black_can_castle_queenside) {
        z_key ^= black_castle_queenside_bitstring;
    }
    if (black_can_castle_kingside) {
        z_key ^= black_castle_kingside_bitstring;
    }

    U64 w_p = Bitboards[WhitePieces];
    U64 b_p = Bitboards[BlackPieces];


    // Iterate through the pieces and xor their positions accordingly
    for (int i = Kings; i <= Pawns; i++) {
        U64 pieces = Bitboards[i] & w_p;

        if (pieces)
            do {
                int index = bitscan_forward(pieces);
                z_key ^= piece_bitstrings[index][WhitePieces][i - 2];
            } while (pieces &= pieces - 1);
    }

    for (int i = Kings; i <= Pawns; i++) {
        U64 pieces = Bitboards[i] & b_p;

        if (pieces)
            do {
                int index = bitscan_forward(pieces);
                z_key ^= piece_bitstrings[index][BlackPieces][i - 2];
            } while (pieces &= pieces - 1);
    }

    // Hash en_passant_square
    if (en_passant_square != -1) {
        // Find the en_passant file
        U64 southrays = rays[South][en_passant_square];
        int index = bitscan_forward(southrays);
        z_key ^= en_passant_bitstrings[index];
    }
}

bool Board::get_current_turn() {
    return current_turn;
}

void Board::print_board() {
    // Debug routine
    // Prints all the bitboards

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


void Board::print_z_key() {
    std::cout << z_key << '\n';
}

bool Board::verify_bitboard() {
    for (int i = Kings; i < Pawns; i++) {
        if (Bitboards[i] & Bitboards[i + 1]) {
            print_board();
            return false;
        }
    }
    if (Bitboards[WhitePieces] & Bitboards[BlackPieces]) {
        print_board();
        return false;
    }
    U64 all_pieces = C64(0);
    for (int i = Kings; i <= Pawns; i++) {
        all_pieces |= Bitboards[i];
    }
    if (all_pieces != (Bitboards[WhitePieces] | Bitboards[BlackPieces])) {
        print_board();
        return false;
    }
    return true;
}


unsigned int Board::find_piece_occupying_sq(int index) {
    // Finds what piece occupies a square
    // This sort of lookup is inherently slow for bitboards

    /*
    // Branchless version
    U64 bit = C64(1) << index;
    return PIECE_QUEEN * ((Bitboards[Queens] & bit) != 0) + PIECE_ROOK * ((Bitboards[Rooks] & bit) != 0) + PIECE_BISHOP * ((Bitboards[Bishops] & bit) != 0) + PIECE_KNIGHT * ((Bitboards[Knights] & bit) != 0) + PIECE_PAWN * ((Bitboards[Pawns] & bit) != 0);
    */

    U64 bit = C64(1) << index;
    if (!((Bitboards[WhitePieces] | Bitboards[BlackPieces]) & bit)) {
        return PIECE_NONE;
    } else if (Bitboards[Pawns] & bit) {
        return PIECE_PAWN;
    } else if (Bitboards[Knights] & bit) {
        return PIECE_KNIGHT;
    } else if (Bitboards[Bishops] & bit) {
        return PIECE_BISHOP;
    } else if (Bitboards[Rooks] & bit) {
        return PIECE_ROOK;
    } else if (Bitboards[Queens] & bit) {
        return PIECE_QUEEN;
    } else if (Bitboards[Kings] & bit) {
        return PIECE_KING;
    } else {
        return PIECE_NONE;
    }
}


unsigned int Board::find_piece_captured(int index) {
    // Similar routine to the above
    // This function does not check if the capture is illegal, make sure to &~friendly_pieces beforehand.
    // Also, it doesn't check king captures

    /*
    U64 bit = C64(1) << index;
    return PIECE_QUEEN * ((Bitboards[Queens] & bit) != 0) + PIECE_ROOK * ((Bitboards[Rooks] & bit) != 0) + PIECE_BISHOP * ((Bitboards[Bishops] & bit) != 0) + PIECE_KNIGHT * ((Bitboards[Knights] & bit) != 0) + PIECE_PAWN * ((Bitboards[Pawns] & bit) != 0);
    */

    U64 bit = C64(1) << index;
    if (!(Bitboards[!current_turn] & bit)) {
        return PIECE_NONE;
    } else if (Bitboards[Pawns] & bit) {
        return PIECE_PAWN;
    } else if (Bitboards[Knights] & bit) {
        return PIECE_KNIGHT;
    } else if (Bitboards[Bishops] & bit) {
        return PIECE_BISHOP;
    } else if (Bitboards[Rooks] & bit) {
        return PIECE_ROOK;
    } else if (Bitboards[Queens] & bit) {
        return PIECE_QUEEN;
    } else {
        return PIECE_NONE;
    }
}

unsigned int Board::find_piece_captured_without_occ(int index) {
    // Same as above, but assumes that at least some piece will be captured
    // This function does not check if the capture is illegal, make sure to &~friendly_pieces beforehand.
    // Also, it doesn't check king captures

    /*
    U64 bit = C64(1) << index;
    return 2 * ((Bitboards[Queens] & bit) != 0) + 3 * ((Bitboards[Rooks] & bit) != 0) + 4 * ((Bitboards[Bishops] & bit) != 0) + 5 * ((Bitboards[Knights] & bit) != 0) + 6 * ((Bitboards[Pawns] & bit) != 0);
     */

    U64 bit = C64(1) << index;
    if (Bitboards[Pawns] & bit) {
        return PIECE_PAWN;
    } else if (Bitboards[Knights] & bit) {
        return PIECE_KNIGHT;
    } else if (Bitboards[Bishops] & bit) {
        return PIECE_BISHOP;
    } else if (Bitboards[Rooks] & bit) {
        return PIECE_ROOK;
    } else if (Bitboards[Queens] & bit) {
        return PIECE_QUEEN;
    } else {
        return PIECE_NONE;
    }
}



// MOVE GENERATION BEGIN


template<MoveGenType gen_type>
void Board::generate_moves(MoveList& moves, bool& is_in_check) {
    // Routine for generating moves

    U64 king_attackers; // Holds opponent pieces attacking the king
    U64 bishop_pinned, rook_pinned; // These hold the pieces pinned by the opponent. 'rook' and 'bishop' indicate the way they are pinned
    int num_attackers; // Number of attackers attacking the king
    U64 block_masks = UniverseBoard; // Holds which squares pieces must go to in order to nullify a check (capture attacker, block check if attacker is Q, B, or R.

    int pinners[8]; // Since the king can only be pinned from the eight directions, hold the index of the possible pinners in this array (use Directions enum to look up with)

    U64 occ = Bitboards[WhitePieces] | Bitboards[BlackPieces]; // BB with all the occupied squares
    U64 friendly_pieces = Bitboards[current_turn];
    int king_index = bitscan_forward(Bitboards[Kings] & friendly_pieces);

    king_attackers = attacks_to(bitscan_forward(Bitboards[Kings] & friendly_pieces), occ); // Find all king_attackers
    num_attackers = pop_count(king_attackers); // Count the number of attackers

    // During the search it may be useful to know whether the player is under check
    // Since we want to avoid a recaculation, we'd like to save the info
    is_in_check = num_attackers >= 1;

    // Generate king moves first
    generate_king_moves<gen_type>(moves, occ, friendly_pieces, king_index, num_attackers);

    if (num_attackers == 1) {
        // In the case of check, we'll need to calculate the block masks
        block_masks = calculate_block_masks(king_attackers);
    } else if (num_attackers > 1) {
        // If num_attackers is 2, then we have a double attack
        // This type of check is unblockable, which means the only way to get out of check is to move the king
        // Since we've already generated the king's moves, we can safely exit generate_moves()
        return;
    }


    // Calculate the pinned pieces
    bishop_pinned = calculate_bishop_pins(pinners, occ, friendly_pieces);
    rook_pinned = calculate_rook_pins(pinners, occ, friendly_pieces);

    // There are two pawn move generators (depending on who's turn it is)
    // This is done for efficiency reasons
    if (current_turn == WHITE) {
        generate_pawn_movesW<gen_type>(moves, block_masks, occ, friendly_pieces, pinners, rook_pinned, bishop_pinned,
                                       king_index);
    } else {
        generate_pawn_movesB<gen_type>(moves, block_masks, occ, friendly_pieces, pinners, rook_pinned, bishop_pinned,
                                       king_index);
    }
    generate_knight_moves<gen_type>(moves, block_masks, occ, friendly_pieces, rook_pinned, bishop_pinned);
    generate_bishop_moves<gen_type>(moves, block_masks, occ, friendly_pieces, pinners, rook_pinned, bishop_pinned,
                                    king_index);
    generate_rook_moves<gen_type>(moves, block_masks, occ, friendly_pieces, pinners, rook_pinned, bishop_pinned,
                                  king_index);
    generate_queen_moves<gen_type>(moves, block_masks, occ, friendly_pieces, pinners, rook_pinned, bishop_pinned,
                                   king_index);
}

template<MoveGenType gen_type>
void Board::generate_moves(MoveList& moves) {
    bool b;
    generate_moves<gen_type>(moves, b);
}


template<MoveGenType gen_type>
inline void
Board::generate_king_moves(MoveList& moves, U64 occ, U64 friendly_pieces, int king_index, int num_attackers) {

    // Castling section

    if (gen_type == ALL_MOVES) {
        if (current_turn == WHITE) {
            if (white_can_castle_queenside && !num_attackers && !(C64(0xE) & occ) && !is_attacked(3, occ) &&
                !is_attacked(2, occ)) {
                moves.push_back(Move(4, 2, MOVE_CASTLING, CASTLE_TYPE_QUEENSIDE, PIECE_KING, PIECE_NONE));
            }

            if (white_can_castle_kingside && !num_attackers && !(C64(0x60) & occ) && !is_attacked(5, occ) &&
                !is_attacked(6, occ)) {
                moves.push_back(Move(4, 6, MOVE_CASTLING, CASTLE_TYPE_KINGSIDE, PIECE_KING, PIECE_NONE));
            }
        } else {
            if (black_can_castle_queenside && !num_attackers && !(C64(0xE00000000000000) & occ) &&
                !is_attacked(59, occ) && !is_attacked(58, occ)) {
                moves.push_back(Move(60, 58, MOVE_CASTLING, CASTLE_TYPE_QUEENSIDE, PIECE_KING, PIECE_NONE));
            }

            if (black_can_castle_kingside && !num_attackers && !(C64(0x6000000000000000) & occ) &&
                !is_attacked(61, occ) && !is_attacked(62, occ)) {
                moves.push_back(Move(60, 62, MOVE_CASTLING, CASTLE_TYPE_KINGSIDE, PIECE_KING, PIECE_NONE));
            }
        }
    }
    // Castling section end

    // Look up possible king moves and remove locations where there's a friendly piece (because you can't capture your own piece)
    U64 move_targets = king_paths[king_index] & ~friendly_pieces;

    // If we only want captures, we'll intersect move_targets with the occupied squares
    if (gen_type == CAPTURES_ONLY) {
        move_targets &= occ;
    }

    // Given a rank in the chess board:
    // R * * k * * * *
    // is_attacked() will be tricked into thinking that the king is blocking the rightmost squares from being attacked
    // In truth, the king cannot move to the right, so we must hide the king from is_attacked()
    U64 occ_without_friendly_king = occ ^ (C64(1) << king_index);

    if (move_targets)
        do {
            int to_index = bitscan_forward(move_targets);

            // King cannot move into check (see above for why occ_without_friendly_king is used instead of normal occ)
            if (!is_attacked(to_index, occ_without_friendly_king)) {
                moves.push_back(Move(king_index, to_index, MOVE_NORMAL, 0, PIECE_KING, find_piece_captured(to_index)));
            }
        } while (move_targets &= move_targets - 1); // Remove the target we just processed from move_target
}


template<MoveGenType gen_type>
inline void
Board::generate_pawn_movesW(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners,
                            U64 rook_pinned, U64 bishop_pinned, int king_index) {

    // First handle pawns that are not pinned
    U64 pawns = Bitboards[Pawns] & friendly_pieces & ~rook_pinned & ~bishop_pinned;

    if (Bitboards[Pawns] & friendly_pieces & ~bishop_pinned) {
        // Generate pawn attacks and pushes for all pawns at the same time

        // East attacks:
        U64 east_attacks = ((pawns << 9) & ~a_file) & Bitboards[BlackPieces];
        east_attacks &= block_check_masks;
        // seperate out promotions
        U64 east_promotion_attacks = east_attacks & eighth_rank;
        U64 east_regular_attacks = east_attacks & ~eighth_rank;


        // West attacks:
        U64 west_attacks = ((pawns << 7) & ~h_file) & Bitboards[BlackPieces];
        west_attacks &= block_check_masks;
        U64 west_promotion_attacks = west_attacks & eighth_rank;
        U64 west_regular_attacks = west_attacks & ~eighth_rank;



        // Serialize into moves:
        if (east_regular_attacks)
            do {
                int to_index = bitscan_forward(east_regular_attacks);
                moves.push_back(Move(to_index - 9, to_index, MOVE_NORMAL, 0, PIECE_PAWN,
                                     find_piece_captured_without_occ(to_index)));
            } while (east_regular_attacks &= east_regular_attacks - 1);

        if (east_promotion_attacks)
            do {
                int to_index = bitscan_forward(east_promotion_attacks);
                auto piece_captured = find_piece_captured_without_occ(to_index);
                moves.push_back(
                        Move(to_index - 9, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
                moves.push_back(
                        Move(to_index - 9, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, piece_captured));
                moves.push_back(
                        Move(to_index - 9, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, piece_captured));
                moves.push_back(
                        Move(to_index - 9, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, piece_captured));
            } while (east_promotion_attacks &= east_promotion_attacks - 1);

        if (west_regular_attacks)
            do {
                int to_index = bitscan_forward(west_regular_attacks);
                moves.push_back(Move(to_index - 7, to_index, MOVE_NORMAL, 0, PIECE_PAWN,
                                     find_piece_captured_without_occ(to_index)));
            } while (west_regular_attacks &= west_regular_attacks - 1);

        if (west_promotion_attacks)
            do {
                int to_index = bitscan_forward(west_promotion_attacks);
                auto piece_captured = find_piece_captured_without_occ(to_index);
                moves.push_back(
                        Move(to_index - 7, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
                moves.push_back(
                        Move(to_index - 7, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, piece_captured));
                moves.push_back(
                        Move(to_index - 7, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, piece_captured));
                moves.push_back(
                        Move(to_index - 7, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, piece_captured));
            } while (west_promotion_attacks &= west_promotion_attacks - 1);


        // Quiet moves:
        if (gen_type == ALL_MOVES) {

            // Add Northern rook pins (only type of pin that pawn_push can move in)
            U64 north_and_south_of_king = rays[North][king_index] | rays[South][king_index];

            // Shift all pawns up and exclude squares that are already occupied
            U64 pawn_regular_pushes =
                    (((pawns | (north_and_south_of_king & Bitboards[Pawns] & rook_pinned)) << 8) & ~eighth_rank) &
                    ~(occ);
            // Push up pawns that can single push and are on the second rank and are not pushing into an occupied square
            U64 pawn_double_pushes = ((pawn_regular_pushes & (first_rank << 16)) << 8) & ~(occ);

            pawn_regular_pushes &= block_check_masks;
            pawn_double_pushes &= block_check_masks;

            U64 pawn_promotion_pushes = ((pawns << 8) & eighth_rank) & ~(occ);
            pawn_promotion_pushes &= block_check_masks;

            // Serialize into moves
            if (pawn_regular_pushes)
                do {
                    int to_index = bitscan_forward(pawn_regular_pushes);
                    moves.push_back(Move(to_index - 8, to_index, MOVE_NORMAL, 0, PIECE_PAWN, 0));
                } while (pawn_regular_pushes &= pawn_regular_pushes - 1);

            if (pawn_double_pushes)
                do {
                    int to_index = bitscan_forward(pawn_double_pushes);
                    moves.push_back(Move(to_index - 16, to_index, MOVE_NORMAL, 0, PIECE_PAWN, 0));
                } while (pawn_double_pushes &= pawn_double_pushes - 1);

            if (pawn_promotion_pushes)
                do {
                    int to_index = bitscan_forward(pawn_promotion_pushes);
                    moves.push_back(Move(to_index - 8, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, 0));
                    moves.push_back(Move(to_index - 8, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, 0));
                    moves.push_back(Move(to_index - 8, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, 0));
                    moves.push_back(Move(to_index - 8, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, 0));
                } while (pawn_promotion_pushes &= pawn_promotion_pushes - 1);
        } // if (include_quiet)
    }

    // Pinned pawns are handled individually:
    U64 pinned_pawn_attacks =
            Bitboards[Pawns] & bishop_pinned; // No rook pinned since attacks can't happen when pinned by rook

    if (pinned_pawn_attacks)
        do {
            int from_index = bitscan_forward(pinned_pawn_attacks);
            U64 move_targets = pawn_attacks[WhitePieces][from_index];
            move_targets &= block_check_masks;
            move_targets &= C64(1) << *(pinners + direction_between[king_index][from_index]);

            if (move_targets) {
                int to_index = bitscan_forward(move_targets);
                auto piece_captured = find_piece_captured_without_occ(to_index);
                if (from_index >= 48) {
                    moves.push_back(
                            Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
                    moves.push_back(
                            Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, piece_captured));
                    moves.push_back(
                            Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
                    moves.push_back(
                            Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
                } else {
                    moves.push_back(Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_PAWN, piece_captured));
                }
            }

        } while (pinned_pawn_attacks &= pinned_pawn_attacks - 1);

    // En Passant:
    if (en_passant_square != -1 && (((C64(1) << en_passant_square) & block_check_masks) ||
                                    (C64(1) << (en_passant_square - 8) & block_check_masks))) {
        U64 en_passant_pawn_source = pawns & pawn_attacks[BlackPieces][en_passant_square];


        if (en_passant_pawn_source)
            do {
                int from_index = bitscan_forward(en_passant_pawn_source);

                // Find if en_passant square has the possibility of being pinned:
                if ((C64(1) << from_index) & (rays[East][king_index] | rays[West][king_index])) {
                    U64 occ_minus_ep_pawns = occ ^ ((C64(1) << (en_passant_square - 8)) | (C64(1) << from_index));
                    if ((get_negative_ray_attacks(king_index, West, occ_minus_ep_pawns) |
                         get_positive_ray_attacks(king_index, East, occ_minus_ep_pawns)) &
                        (Bitboards[Rooks] | Bitboards[Queens]) & Bitboards[!current_turn]) {
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
            moves.push_back(Move(bitscan_forward(pinned_en_passant), en_passant_square, MOVE_ENPASSANT, 0, PIECE_PAWN,
                                 PIECE_PAWN));
        }
    }
}

template<MoveGenType gen_type>
inline void
Board::generate_pawn_movesB(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners,
                            U64 rook_pinned, U64 bishop_pinned, int king_index) {
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
        if (east_regular_attacks)
            do {
                int to_index = bitscan_forward(east_regular_attacks);
                moves.push_back(Move(to_index + 7, to_index, MOVE_NORMAL, 0, PIECE_PAWN,
                                     find_piece_captured_without_occ(to_index)));
            } while (east_regular_attacks &= east_regular_attacks - 1);

        if (east_promotion_attacks)
            do {
                int to_index = bitscan_forward(east_promotion_attacks);
                auto piece_captured = find_piece_captured_without_occ(to_index);
                moves.push_back(
                        Move(to_index + 7, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
                moves.push_back(
                        Move(to_index + 7, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, piece_captured));
                moves.push_back(
                        Move(to_index + 7, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, piece_captured));
                moves.push_back(
                        Move(to_index + 7, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, piece_captured));
            } while (east_promotion_attacks &= east_promotion_attacks - 1);

        if (west_regular_attacks)
            do {
                int to_index = bitscan_forward(west_regular_attacks);
                moves.push_back(Move(to_index + 9, to_index, MOVE_NORMAL, 0, PIECE_PAWN,
                                     find_piece_captured_without_occ(to_index)));
            } while (west_regular_attacks &= west_regular_attacks - 1);

        if (west_promotion_attacks)
            do {
                int to_index = bitscan_forward(west_promotion_attacks);
                auto piece_captured = find_piece_captured_without_occ(to_index);
                moves.push_back(
                        Move(to_index + 9, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
                moves.push_back(
                        Move(to_index + 9, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, piece_captured));
                moves.push_back(
                        Move(to_index + 9, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, piece_captured));
                moves.push_back(
                        Move(to_index + 9, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, piece_captured));
            } while (west_promotion_attacks &= west_promotion_attacks - 1);


        // Quiet moves:
        if (gen_type == ALL_MOVES) {
            // Add Southern rook pins (only type of pin that pawn_push can move in)
            U64 north_and_south_of_king = rays[North][king_index] | rays[South][king_index];

            U64 pawn_regular_pushes =
                    (((pawns | (north_and_south_of_king & Bitboards[Pawns] & rook_pinned)) >> 8) & ~first_rank) &
                    ~(occ);
            U64 pawn_double_pushes = ((pawn_regular_pushes & (eighth_rank >> 16)) >> 8) & ~(occ);

            pawn_regular_pushes &= block_check_masks;
            pawn_double_pushes &= block_check_masks;

            U64 pawn_promotion_pushes = ((pawns >> 8) & first_rank) & ~(occ);
            pawn_promotion_pushes &= block_check_masks;


            if (pawn_regular_pushes)
                do {
                    int to_index = bitscan_forward(pawn_regular_pushes);
                    moves.push_back(Move(to_index + 8, to_index, MOVE_NORMAL, 0, PIECE_PAWN, 0));
                } while (pawn_regular_pushes &= pawn_regular_pushes - 1);

            if (pawn_double_pushes)
                do {
                    int to_index = bitscan_forward(pawn_double_pushes);
                    moves.push_back(Move(to_index + 16, to_index, MOVE_NORMAL, 0, PIECE_PAWN, 0));
                } while (pawn_double_pushes &= pawn_double_pushes - 1);

            if (pawn_promotion_pushes)
                do {
                    int to_index = bitscan_forward(pawn_promotion_pushes);
                    moves.push_back(Move(to_index + 8, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, 0));
                    moves.push_back(Move(to_index + 8, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, 0));
                    moves.push_back(Move(to_index + 8, to_index, MOVE_PROMOTION, PROMOTE_TO_ROOK, PIECE_PAWN, 0));
                    moves.push_back(Move(to_index + 8, to_index, MOVE_PROMOTION, PROMOTE_TO_QUEEN, PIECE_PAWN, 0));
                } while (pawn_promotion_pushes &= pawn_promotion_pushes - 1);
        } // if (include_quiet)
    }

    // Pinned pawns are handled individually:
    U64 pinned_pawn_attacks =
            Bitboards[Pawns] & bishop_pinned; // No rook pinned since attacks can't happen when pinned by rook

    if (pinned_pawn_attacks)
        do {
            int from_index = bitscan_forward(pinned_pawn_attacks);
            U64 move_targets = pawn_attacks[BlackPieces][from_index];
            move_targets &= block_check_masks;
            move_targets &= C64(1) << *(pinners + direction_between[king_index][from_index]);

            if (move_targets) {
                int to_index = bitscan_forward(move_targets);
                auto piece_captured = find_piece_captured_without_occ(to_index);
                if (from_index <= 7) {
                    moves.push_back(
                            Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
                    moves.push_back(
                            Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_BISHOP, PIECE_PAWN, piece_captured));
                    moves.push_back(
                            Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
                    moves.push_back(
                            Move(from_index, to_index, MOVE_PROMOTION, PROMOTE_TO_KNIGHT, PIECE_PAWN, piece_captured));
                } else {
                    moves.push_back(Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_PAWN, piece_captured));
                }
            }

        } while (pinned_pawn_attacks &= pinned_pawn_attacks - 1);

    // En Passant:
    if (en_passant_square != -1 && (((C64(1) << en_passant_square) & block_check_masks) ||
                                    (C64(1) << (en_passant_square + 8) & block_check_masks))) {
        U64 en_passant_pawn_source = pawns & pawn_attacks[WhitePieces][en_passant_square];

        if (en_passant_pawn_source)
            do {
                int from_index = bitscan_forward(en_passant_pawn_source);

                // Find if en_passant square has the possibility of being pinned:
                if ((C64(1) << from_index) & (rays[East][king_index] | rays[West][king_index])) {
                    U64 occ_minus_ep_pawns = occ ^ ((C64(1) << (en_passant_square + 8)) | (C64(1) << from_index));
                    if ((get_negative_ray_attacks(king_index, West, occ_minus_ep_pawns) |
                         get_positive_ray_attacks(king_index, East, occ_minus_ep_pawns)) &
                        (Bitboards[Rooks] | Bitboards[Queens]) & Bitboards[!current_turn]) {
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
            moves.push_back(Move(bitscan_forward(pinned_en_passant), en_passant_square, MOVE_ENPASSANT, 0, PIECE_PAWN,
                                 PIECE_PAWN));
        }
    }
}


template<MoveGenType gen_type>
inline void
Board::generate_knight_moves(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, U64 rook_pinned,
                             U64 bishop_pinned) {
    U64 knights = Bitboards[Knights] & friendly_pieces & ~rook_pinned &
                  ~bishop_pinned; // Knights can't move at all when pinned

    if (knights)
        do {
            int from_index = bitscan_forward(knights);
            U64 move_targets = knight_paths[from_index] & ~friendly_pieces;
            move_targets &= block_check_masks;

            if (gen_type == CAPTURES_ONLY) {
                move_targets &= occ;
            }

            if (move_targets)
                do {
                    int to_index = bitscan_forward(move_targets);
                    moves.push_back(
                            Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_KNIGHT, find_piece_captured(to_index)));
                } while (move_targets &= move_targets - 1);

        } while (knights &= knights - 1);
}


template<MoveGenType gen_type>
inline void
Board::generate_bishop_moves(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners,
                             U64 rook_pinned, U64 bishop_pinned, int king_index) {
    U64 bishops = Bitboards[Bishops] & friendly_pieces & ~rook_pinned &
                  ~bishop_pinned; // Bishops can't move when pinned by a rook
    U64 bishops_bishop_pinned = Bitboards[Bishops] & bishop_pinned;

    if (bishops)
        do {
            int from_index = bitscan_forward(bishops);
            U64 move_targets = bishop_attacks(from_index, occ);
            move_targets &= ~friendly_pieces;
            move_targets &= block_check_masks;

            if (gen_type == CAPTURES_ONLY) {
                move_targets &= occ;
            }

            if (move_targets)
                do {
                    int to_index = bitscan_forward(move_targets);
                    moves.push_back(
                            Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_BISHOP, find_piece_captured(to_index)));
                } while (move_targets &= move_targets - 1);

        } while (bishops &= bishops - 1);

    if (bishops_bishop_pinned)
        do {
            int from_index = bitscan_forward(bishops_bishop_pinned);
            U64 move_targets = bishop_attacks(from_index, occ);
            move_targets &= block_check_masks;
            move_targets &= in_between_mask(king_index, *(pinners + direction_between[king_index][from_index]));

            if (gen_type == CAPTURES_ONLY) {
                move_targets &= occ;
            }

            if (move_targets)
                do {
                    int to_index = bitscan_forward(move_targets);
                    moves.push_back(
                            Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_BISHOP, find_piece_captured(to_index)));
                } while (move_targets &= move_targets - 1);

        } while (bishops_bishop_pinned &= bishops_bishop_pinned - 1);
}


template<MoveGenType gen_type>
inline void
Board::generate_rook_moves(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners,
                           U64 rook_pinned, U64 bishop_pinned, int king_index) {
    U64 rooks = Bitboards[Rooks] & friendly_pieces & ~rook_pinned & ~bishop_pinned;
    U64 rooks_rook_pinned = Bitboards[Rooks] & rook_pinned;

    if (rooks)
        do {
            int from_index = bitscan_forward(rooks);
            U64 move_targets = rook_attacks(from_index, occ);
            move_targets &= ~friendly_pieces;
            move_targets &= block_check_masks;

            if (gen_type == CAPTURES_ONLY) {
                move_targets &= occ;
            }

            if (move_targets)
                do {
                    int to_index = bitscan_forward(move_targets);
                    moves.push_back(
                            Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_ROOK, find_piece_captured(to_index)));
                } while (move_targets &= move_targets - 1);

        } while (rooks &= rooks - 1);

    if (rooks_rook_pinned)
        do {
            int from_index = bitscan_forward(rooks_rook_pinned);
            U64 move_targets = rook_attacks(from_index, occ);
            move_targets &= block_check_masks;
            move_targets &= in_between_mask(king_index, *(pinners + direction_between[king_index][from_index]));

            if (gen_type == CAPTURES_ONLY) {
                move_targets &= occ;
            }

            if (move_targets)
                do {
                    int to_index = bitscan_forward(move_targets);
                    moves.push_back(
                            Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_ROOK, find_piece_captured(to_index)));
                } while (move_targets &= move_targets - 1);

        } while (rooks_rook_pinned &= rooks_rook_pinned - 1);
}


template<MoveGenType gen_type>
inline void
Board::generate_queen_moves(MoveList& moves, U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners,
                            U64 rook_pinned, U64 bishop_pinned, int king_index) {
    U64 queens = Bitboards[Queens] & friendly_pieces & ~rook_pinned & ~bishop_pinned;
    U64 queens_pinned = Bitboards[Queens] & (rook_pinned | bishop_pinned);

    if (queens)
        do {
            int from_index = bitscan_forward(queens);
            U64 move_targets = bishop_attacks(from_index, occ) | rook_attacks(from_index, occ);
            move_targets &= ~friendly_pieces;
            move_targets &= block_check_masks;

            if (gen_type == CAPTURES_ONLY) {
                move_targets &= occ;
            }

            if (move_targets)
                do {
                    int to_index = bitscan_forward(move_targets);
                    moves.push_back(
                            Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_QUEEN, find_piece_captured(to_index)));
                } while (move_targets &= move_targets - 1);

        } while (queens &= queens - 1);

    if (queens_pinned)
        do {
            int from_index = bitscan_forward(queens_pinned);
            U64 move_targets = bishop_attacks(from_index, occ) | rook_attacks(from_index, occ);
            move_targets &= block_check_masks;
            move_targets &= in_between_mask(king_index, *(pinners + direction_between[king_index][from_index]));

            if (gen_type == CAPTURES_ONLY) {
                move_targets &= occ;
            }

            if (move_targets)
                do {
                    int to_index = bitscan_forward(move_targets);
                    moves.push_back(
                            Move(from_index, to_index, MOVE_NORMAL, 0, PIECE_QUEEN, find_piece_captured(to_index)));
                } while (move_targets &= move_targets - 1);

        } while (queens_pinned &= queens_pinned - 1);
}


// Mobility section

template<MoveGenType gen_type>
int Board::calculate_mobility(bool& is_in_check) {
    // Routine for generating moves

    int move_count = 0;

    U64 king_attackers; // Holds opponent pieces attacking the king
    U64 bishop_pinned, rook_pinned; // These hold the pieces pinned by the opponent. 'rook' and 'bishop' indicate the way they are pinned
    int num_attackers; // Number of attackers attacking the king
    U64 block_masks = UniverseBoard; // Holds which squares pieces must go to in order to nullify a check (capture attacker, block check if attacker is Q, B, or R.

    int pinners[8]; // Since the king can only be pinned from the eight directions, hold the index of the possible pinners in this array (use Directions enum to look up with)

    U64 occ = Bitboards[WhitePieces] | Bitboards[BlackPieces]; // BB with all the occupied squares
    U64 friendly_pieces = Bitboards[current_turn];
    int king_index = bitscan_forward(Bitboards[Kings] & friendly_pieces);

    king_attackers = attacks_to(bitscan_forward(Bitboards[Kings] & friendly_pieces), occ); // Find all king_attackers
    num_attackers = pop_count(king_attackers); // Count the number of attackers

    // During the search it may be useful to know whether the player is under check
    // Since we want to avoid a recaculation, we'd like to save the info
    is_in_check = num_attackers >= 1;

    // Generate king moves first
    move_count += calculate_king_mobility<gen_type>(occ, friendly_pieces, king_index, num_attackers);

    if (num_attackers == 1) {
        // In the case of check, we'll need to calculate the block masks
        block_masks = calculate_block_masks(king_attackers);
    } else if (num_attackers > 1) {
        // If num_attackers is 2, then we have a double attack
        // This type of check is unblockable, which means the only way to get out of check is to move the king
        // Since we've already generated the king's moves, we can safely exit generate_moves()
        return move_count;
    }


    // Calculate the pinned pieces
    bishop_pinned = calculate_bishop_pins(pinners, occ, friendly_pieces);
    rook_pinned = calculate_rook_pins(pinners, occ, friendly_pieces);

    // There are two pawn move generators (depending on who's turn it is)
    // This is done for efficiency reasons
    if (current_turn == WHITE) {
        move_count += calculate_pawn_mobilityW<gen_type>(block_masks, occ, friendly_pieces, pinners, rook_pinned,
                                                         bishop_pinned, king_index);
    } else {
        move_count += calculate_pawn_mobilityB<gen_type>(block_masks, occ, friendly_pieces, pinners, rook_pinned,
                                                         bishop_pinned, king_index);
    }
    move_count += calculate_knight_mobility<gen_type>(block_masks, occ, friendly_pieces, rook_pinned, bishop_pinned);
    move_count += calculate_bishop_mobility<gen_type>(block_masks, occ, friendly_pieces, pinners, rook_pinned,
                                                      bishop_pinned, king_index);
    move_count += calculate_rook_mobility<gen_type>(block_masks, occ, friendly_pieces, pinners, rook_pinned,
                                                    bishop_pinned, king_index);
    move_count += calculate_queen_mobility<gen_type>(block_masks, occ, friendly_pieces, pinners, rook_pinned,
                                                     bishop_pinned, king_index);

    return move_count;
}

template<MoveGenType gen_type>
int Board::calculate_mobility() {
    bool b;
    return calculate_mobility<gen_type>(b);
}


template<MoveGenType gen_type>
inline int Board::calculate_king_mobility(U64 occ, U64 friendly_pieces, int king_index, int num_attackers) {

    // Castling section

    int move_count = 0;

    if (gen_type == ALL_MOVES) {
        if (current_turn == WHITE) {
            if (white_can_castle_queenside && !num_attackers && !(C64(0xE) & occ) && !is_attacked(3, occ) &&
                !is_attacked(2, occ)) {
                move_count++;
            }

            if (white_can_castle_kingside && !num_attackers && !(C64(0x60) & occ) && !is_attacked(5, occ) &&
                !is_attacked(6, occ)) {
                move_count++;
            }
        } else {
            if (black_can_castle_queenside && !num_attackers && !(C64(0xE00000000000000) & occ) &&
                !is_attacked(59, occ) && !is_attacked(58, occ)) {
                move_count++;
            }

            if (black_can_castle_kingside && !num_attackers && !(C64(0x6000000000000000) & occ) &&
                !is_attacked(61, occ) && !is_attacked(62, occ)) {
                move_count++;
            }
        }
    }
    // Castling section end

    // Look up possible king moves and remove locations where there's a friendly piece (because you can't capture your own piece)
    U64 move_targets = king_paths[king_index] & ~friendly_pieces;

    // If we only want captures, we'll intersect move_targets with the occupied squares
    if (gen_type == CAPTURES_ONLY) {
        move_targets &= occ;
    }

    // Given a rank in the chess board:
    // R * * k * * * *
    // is_attacked() will be tricked into thinking that the king is blocking the rightmost squares from being attacked
    // In truth, the king cannot move to the right, so we must hide the king from is_attacked()
    U64 occ_without_friendly_king = occ ^ (C64(1) << king_index);

    if (move_targets)
        do {
            int to_index = bitscan_forward(move_targets);

            // King cannot move into check (see above for why occ_without_friendly_king is used instead of normal occ)
            if (!is_attacked(to_index, occ_without_friendly_king)) {
                move_count++;
            }
        } while (move_targets &= move_targets - 1); // Remove the target we just processed from move_target

    return move_count;
}


template<MoveGenType gen_type>
inline int
Board::calculate_pawn_mobilityW(U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners, U64 rook_pinned,
                                U64 bishop_pinned, int king_index) {

    int move_count = 0;

    // First handle pawns that are not pinned
    U64 pawns = Bitboards[Pawns] & friendly_pieces & ~rook_pinned & ~bishop_pinned;

    if (Bitboards[Pawns] & friendly_pieces & ~bishop_pinned) {
        // Generate pawn attacks and pushes for all pawns at the same time

        // East attacks:
        U64 east_attacks = ((pawns << 9) & ~a_file) & Bitboards[BlackPieces];
        east_attacks &= block_check_masks;
        // seperate out promotions
        U64 east_promotion_attacks = east_attacks & eighth_rank;
        U64 east_regular_attacks = east_attacks & ~eighth_rank;


        // West attacks:
        U64 west_attacks = ((pawns << 7) & ~h_file) & Bitboards[BlackPieces];
        west_attacks &= block_check_masks;
        U64 west_promotion_attacks = west_attacks & eighth_rank;
        U64 west_regular_attacks = west_attacks & ~eighth_rank;


        move_count += pop_count(east_regular_attacks);

        move_count += pop_count(east_promotion_attacks) * 4;

        move_count += pop_count(west_regular_attacks);

        move_count += pop_count(west_promotion_attacks) * 4;


        // Quiet moves:
        if (gen_type == ALL_MOVES) {

            // Add Northern rook pins (only type of pin that pawn_push can move in)
            U64 north_and_south_of_king = rays[North][king_index] | rays[South][king_index];

            // Shift all pawns up and exclude squares that are already occupied
            U64 pawn_regular_pushes =
                    (((pawns | (north_and_south_of_king & Bitboards[Pawns] & rook_pinned)) << 8) & ~eighth_rank) &
                    ~(occ);
            // Push up pawns that can single push and are on the second rank and are not pushing into an occupied square
            U64 pawn_double_pushes = ((pawn_regular_pushes & (first_rank << 16)) << 8) & ~(occ);

            pawn_regular_pushes &= block_check_masks;
            pawn_double_pushes &= block_check_masks;

            U64 pawn_promotion_pushes = ((pawns << 8) & eighth_rank) & ~(occ);
            pawn_promotion_pushes &= block_check_masks;

            move_count += pop_count(pawn_regular_pushes | pawn_double_pushes);

            move_count += pop_count(pawn_promotion_pushes) * 4;
        } // if (include_quiet)
    }

    // Pinned pawns are handled individually:
    U64 pinned_pawn_attacks =
            Bitboards[Pawns] & bishop_pinned; // No rook pinned since attacks can't happen when pinned by rook

    if (pinned_pawn_attacks)
        do {
            int from_index = bitscan_forward(pinned_pawn_attacks);
            U64 move_targets = pawn_attacks[WhitePieces][from_index];
            move_targets &= block_check_masks;
            move_targets &= C64(1) << *(pinners + direction_between[king_index][from_index]);

            if (move_targets) {
                if (from_index >= 48) {
                    move_count += 4;
                } else {
                    move_count++;
                }
            }

        } while (pinned_pawn_attacks &= pinned_pawn_attacks - 1);

    // En Passant:
    if (en_passant_square != -1 && (((C64(1) << en_passant_square) & block_check_masks) ||
                                    (C64(1) << (en_passant_square - 8) & block_check_masks))) {
        U64 en_passant_pawn_source = pawns & pawn_attacks[BlackPieces][en_passant_square];


        if (en_passant_pawn_source)
            do {
                int from_index = bitscan_forward(en_passant_pawn_source);

                // Find if en_passant square has the possibility of being pinned:
                if ((C64(1) << from_index) & (rays[East][king_index] | rays[West][king_index])) {
                    U64 occ_minus_ep_pawns = occ ^ ((C64(1) << (en_passant_square - 8)) | (C64(1) << from_index));
                    if ((get_negative_ray_attacks(king_index, West, occ_minus_ep_pawns) |
                         get_positive_ray_attacks(king_index, East, occ_minus_ep_pawns)) &
                        (Bitboards[Rooks] | Bitboards[Queens]) & Bitboards[!current_turn]) {
                        continue;
                    }
                }

                move_count++;
            } while (en_passant_pawn_source &= en_passant_pawn_source - 1);

        // En Passant pins:
        U64 positive_diag_rays_from_king = rays[NorthWest][king_index] | rays[NorthEast][king_index];
        U64 en_passants_along_pin_path = positive_diag_rays_from_king & (C64(1) << en_passant_square);
        U64 pinned_en_passant = Bitboards[Pawns] & bishop_pinned & pawn_attacks[BlackPieces][en_passant_square];
        pinned_en_passant &= block_check_masks;

        if (pinned_en_passant && en_passants_along_pin_path) {
            move_count++;
        }
    }

    return move_count;
}

template<MoveGenType gen_type>
inline int
Board::calculate_pawn_mobilityB(U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners, U64 rook_pinned,
                                U64 bishop_pinned, int king_index) {

    int move_count = 0;

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


        move_count += pop_count(east_regular_attacks);

        move_count += pop_count(east_promotion_attacks) * 4;

        move_count += pop_count(west_regular_attacks);

        move_count += pop_count(west_promotion_attacks) * 4;


        // Quiet moves:
        if (gen_type == ALL_MOVES) {
            // Add Southern rook pins (only type of pin that pawn_push can move in)
            U64 north_and_south_of_king = rays[North][king_index] | rays[South][king_index];

            U64 pawn_regular_pushes =
                    (((pawns | (north_and_south_of_king & Bitboards[Pawns] & rook_pinned)) >> 8) & ~first_rank) &
                    ~(occ);
            U64 pawn_double_pushes = ((pawn_regular_pushes & (eighth_rank >> 16)) >> 8) & ~(occ);

            pawn_regular_pushes &= block_check_masks;
            pawn_double_pushes &= block_check_masks;

            U64 pawn_promotion_pushes = ((pawns >> 8) & first_rank) & ~(occ);
            pawn_promotion_pushes &= block_check_masks;


            move_count += pop_count(pawn_regular_pushes | pawn_double_pushes);

            move_count += pop_count(pawn_promotion_pushes) * 4;
        } // if (include_quiet)
    }

    // Pinned pawns are handled individually:
    U64 pinned_pawn_attacks =
            Bitboards[Pawns] & bishop_pinned; // No rook pinned since attacks can't happen when pinned by rook

    if (pinned_pawn_attacks)
        do {
            int from_index = bitscan_forward(pinned_pawn_attacks);
            U64 move_targets = pawn_attacks[BlackPieces][from_index];
            move_targets &= block_check_masks;
            move_targets &= C64(1) << *(pinners + direction_between[king_index][from_index]);

            if (move_targets) {
                if (from_index <= 7) {
                    move_count += 4;
                } else {
                    move_count++;
                }
            }

        } while (pinned_pawn_attacks &= pinned_pawn_attacks - 1);

    // En Passant:
    if (en_passant_square != -1 && (((C64(1) << en_passant_square) & block_check_masks) ||
                                    (C64(1) << (en_passant_square + 8) & block_check_masks))) {
        U64 en_passant_pawn_source = pawns & pawn_attacks[WhitePieces][en_passant_square];

        if (en_passant_pawn_source)
            do {
                int from_index = bitscan_forward(en_passant_pawn_source);

                // Find if en_passant square has the possibility of being pinned:
                if ((C64(1) << from_index) & (rays[East][king_index] | rays[West][king_index])) {
                    U64 occ_minus_ep_pawns = occ ^ ((C64(1) << (en_passant_square + 8)) | (C64(1) << from_index));
                    if ((get_negative_ray_attacks(king_index, West, occ_minus_ep_pawns) |
                         get_positive_ray_attacks(king_index, East, occ_minus_ep_pawns)) &
                        (Bitboards[Rooks] | Bitboards[Queens]) & Bitboards[!current_turn]) {
                        continue;
                    }
                }

                move_count++;
            } while (en_passant_pawn_source &= en_passant_pawn_source - 1);

        // En Passant pins:
        U64 negative_diag_rays_from_king = rays[SouthWest][king_index] | rays[SouthEast][king_index];
        U64 en_passants_along_pin_path = negative_diag_rays_from_king & (C64(1) << en_passant_square);
        U64 pinned_en_passant = Bitboards[Pawns] & bishop_pinned & pawn_attacks[WhitePieces][en_passant_square];
        pinned_en_passant &= block_check_masks;

        if (pinned_en_passant && en_passants_along_pin_path) {
            move_count++;
        }
    }
    return move_count;
}


template<MoveGenType gen_type>
inline int Board::calculate_knight_mobility(U64 block_check_masks, U64 occ, U64 friendly_pieces, U64 rook_pinned,
                                            U64 bishop_pinned) {

    int move_count = 0;
    U64 knights = Bitboards[Knights] & friendly_pieces & ~rook_pinned &
                  ~bishop_pinned; // Knights can't move at all when pinned


    if (knights)
        do {
            int from_index = bitscan_forward(knights);
            U64 move_targets = knight_paths[from_index] & ~friendly_pieces;
            move_targets &= block_check_masks;

            if (gen_type == CAPTURES_ONLY) {
                move_targets &= occ;
            }

            move_count += pop_count(move_targets);

        } while (knights &= knights - 1);

    return move_count;
}


template<MoveGenType gen_type>
inline int
Board::calculate_bishop_mobility(U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners, U64 rook_pinned,
                                 U64 bishop_pinned, int king_index) {
    int move_count = 0;
    U64 bishops = Bitboards[Bishops] & friendly_pieces & ~rook_pinned &
                  ~bishop_pinned; // Bishops can't move when pinned by a rook
    U64 bishops_bishop_pinned = Bitboards[Bishops] & bishop_pinned;

    if (bishops)
        do {
            int from_index = bitscan_forward(bishops);
            U64 move_targets = bishop_attacks(from_index, occ);
            move_targets &= ~friendly_pieces;
            move_targets &= block_check_masks;

            if (gen_type == CAPTURES_ONLY) {
                move_targets &= occ;
            }

            move_count += pop_count(move_targets);

        } while (bishops &= bishops - 1);

    if (bishops_bishop_pinned)
        do {
            int from_index = bitscan_forward(bishops_bishop_pinned);
            U64 move_targets = bishop_attacks(from_index, occ);
            move_targets &= block_check_masks;
            move_targets &= in_between_mask(king_index, *(pinners + direction_between[king_index][from_index]));

            if (gen_type == CAPTURES_ONLY) {
                move_targets &= occ;
            }

            move_count += pop_count(move_targets);

        } while (bishops_bishop_pinned &= bishops_bishop_pinned - 1);

    return move_count;
}


template<MoveGenType gen_type>
inline int
Board::calculate_rook_mobility(U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners, U64 rook_pinned,
                               U64 bishop_pinned, int king_index) {
    int move_count = 0;
    U64 rooks = Bitboards[Rooks] & friendly_pieces & ~rook_pinned & ~bishop_pinned;
    U64 rooks_rook_pinned = Bitboards[Rooks] & rook_pinned;

    if (rooks)
        do {
            int from_index = bitscan_forward(rooks);
            U64 move_targets = rook_attacks(from_index, occ);
            move_targets &= ~friendly_pieces;
            move_targets &= block_check_masks;

            if (gen_type == CAPTURES_ONLY) {
                move_targets &= occ;
            }

            move_count += pop_count(move_targets);

        } while (rooks &= rooks - 1);

    if (rooks_rook_pinned)
        do {
            int from_index = bitscan_forward(rooks_rook_pinned);
            U64 move_targets = rook_attacks(from_index, occ);
            move_targets &= block_check_masks;
            move_targets &= in_between_mask(king_index, *(pinners + direction_between[king_index][from_index]));

            if (gen_type == CAPTURES_ONLY) {
                move_targets &= occ;
            }

            move_count += pop_count(move_targets);

        } while (rooks_rook_pinned &= rooks_rook_pinned - 1);

    return move_count;
}


template<MoveGenType gen_type>
inline int
Board::calculate_queen_mobility(U64 block_check_masks, U64 occ, U64 friendly_pieces, int* pinners, U64 rook_pinned,
                                U64 bishop_pinned, int king_index) {
    int move_count = 0;
    U64 queens = Bitboards[Queens] & friendly_pieces & ~rook_pinned & ~bishop_pinned;
    U64 queens_pinned = Bitboards[Queens] & (rook_pinned | bishop_pinned);

    if (queens)
        do {
            int from_index = bitscan_forward(queens);
            U64 move_targets = bishop_attacks(from_index, occ) | rook_attacks(from_index, occ);
            move_targets &= ~friendly_pieces;
            move_targets &= block_check_masks;

            if (gen_type == CAPTURES_ONLY) {
                move_targets &= occ;
            }

            move_count += pop_count(move_targets);

        } while (queens &= queens - 1);

    if (queens_pinned)
        do {
            int from_index = bitscan_forward(queens_pinned);
            U64 move_targets = bishop_attacks(from_index, occ) | rook_attacks(from_index, occ);
            move_targets &= block_check_masks;
            move_targets &= in_between_mask(king_index, *(pinners + direction_between[king_index][from_index]));

            if (gen_type == CAPTURES_ONLY) {
                move_targets &= occ;
            }

            move_count += pop_count(move_targets);

        } while (queens_pinned &= queens_pinned - 1);

    return move_count;
}


// Legality section

inline U64 Board::attacks_to(int index, U64 occ) {
    // Important: this function ->does not<- factor in king attacks
    U64 enemy_pawns = Bitboards[Pawns] & Bitboards[!current_turn];
    U64 enemy_knights = Bitboards[Knights] & Bitboards[!current_turn];
    U64 enemy_bishops_queens = (Bitboards[Bishops] | Bitboards[Queens]) & Bitboards[!current_turn];
    U64 enemy_rooks_queens = (Bitboards[Rooks] | Bitboards[Queens]) & Bitboards[!current_turn];

    return (pawn_attacks[current_turn][index] & enemy_pawns)
           | (knight_paths[index] & enemy_knights)
           | (bishop_attacks(index, occ) & enemy_bishops_queens)
           | (rook_attacks(index, occ) & enemy_rooks_queens);
}

inline int Board::is_attacked(int index, U64 occ) {
    // Important: this function factors in king attacks
    U64 enemy_king = Bitboards[Kings] & Bitboards[!current_turn];
    U64 enemy_pawns = Bitboards[Pawns] & Bitboards[!current_turn];
    U64 enemy_knights = Bitboards[Knights] & Bitboards[!current_turn];
    U64 enemy_bishops_queens = (Bitboards[Bishops] | Bitboards[Queens]) & Bitboards[!current_turn];
    U64 enemy_rooks_queens = (Bitboards[Rooks] | Bitboards[Queens]) & Bitboards[!current_turn];

    return (pawn_attacks[current_turn][index] & enemy_pawns)
           || (knight_paths[index] & enemy_knights)
           || (bishop_attacks(index, occ) & enemy_bishops_queens)
           || (rook_attacks(index, occ) & enemy_rooks_queens)
           || (king_paths[index] & enemy_king);
}


inline U64 Board::calculate_block_masks(U64 king_attacker) {
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

inline U64 Board::calculate_bishop_pins(int* pinners, U64 occ, U64 friendly_pieces) {
    // Make sure to pass in int arr[8], otherwise segfault
    int king_index = bitscan_forward(Bitboards[Kings] & friendly_pieces);
    U64 pinner = xray_bishop_attacks(king_index, occ, friendly_pieces) & (Bitboards[Bishops] | Bitboards[Queens]) &
                 Bitboards[!current_turn];
    U64 pinned = 0;
    while (pinner) {
        int sq = bitscan_forward(pinner);
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
    U64 pinner = xray_rook_attacks(king_index, occ, friendly_pieces) & (Bitboards[Rooks] | Bitboards[Queens]) &
                 Bitboards[!current_turn];
    U64 pinned = 0;
    while (pinner) {
        int sq = bitscan_forward(pinner);
        // Save the index of pinner so we don't have to recalculate later:
        *(pinners + direction_between[king_index][sq]) = sq;

        pinned |= in_between_mask(king_index, sq) & friendly_pieces;
        pinner &= pinner - 1;
    }
    return pinned;
}


void Board::make_move(Move move) {
    move_data m = {move, white_can_castle_queenside, white_can_castle_kingside, black_can_castle_queenside,
                   black_can_castle_kingside, en_passant_square, z_key, false, halfmove_counter};
    move_stack.push_back(m);

    // Actual act of making move:
    int move_from_index = move.get_from();
    int move_to_index = move.get_to();
    U64 from_bb = C64(1) << move_from_index;
    U64 to_bb = C64(1) << move_to_index;



    // Set out the captured squares first
    if (move.get_piece_captured() != PIECE_NONE && move.get_special_flag() != MOVE_ENPASSANT) {
        Bitboards[move.get_piece_captured()] ^= to_bb;
        Bitboards[!current_turn] ^= to_bb;

        // Take away the piece values for the side that pieces were captured
        piece_values[!current_turn] -= piece_to_value[move.get_piece_captured()];

        // Update piece_count
//        piece_count[!current_turn][move.get_piece_captured() - 2] -= 1;

        // Update zobrist key for capture
        z_key ^= piece_bitstrings[move_to_index][!current_turn][move.get_piece_captured() -
                                                                2]; // Must -2 because pieces start at third index, not first
        // Take away piece square value for captured piece
        piece_square_values_m[!current_turn] -= lookup_ps_table_m(move_to_index, move.get_piece_captured(),
                                                                  !current_turn);
        piece_square_values_e[!current_turn] -= lookup_ps_table_e(move_to_index, move.get_piece_captured(),
                                                                  !current_turn);

        // Reset halfmove counter:
        halfmove_counter = -1;
    }
    // Flip the occupacy of the from square and to square
    Bitboards[current_turn] ^= from_bb | to_bb;
    Bitboards[move.get_piece_moved()] ^= from_bb | to_bb;

    // Update zobrist keys
    z_key ^= piece_bitstrings[move_from_index][current_turn][move.get_piece_moved() - 2];
    z_key ^= piece_bitstrings[move_to_index][current_turn][move.get_piece_moved() - 2];


    piece_square_values_m[current_turn] -= lookup_ps_table_m(move_from_index, move.get_piece_moved(), current_turn);
    piece_square_values_e[current_turn] -= lookup_ps_table_e(move_from_index, move.get_piece_moved(), current_turn);

    piece_square_values_m[current_turn] += lookup_ps_table_m(move_to_index, move.get_piece_moved(), current_turn);
    piece_square_values_e[current_turn] += lookup_ps_table_e(move_to_index, move.get_piece_moved(), current_turn);


    // Special move handling now:

    switch (move.get_special_flag()) {
        case MOVE_NORMAL: {
            break;
        }
        case MOVE_CASTLING: {
            U64 rook_bits;
            int rook_from_index, rook_to_index;
            if (move.get_castle_type() == CASTLE_TYPE_KINGSIDE) {
                rook_from_index = 7;
                rook_to_index = 5;
            } else {
                rook_from_index = 0;
                rook_to_index = 3;
            }
            if (current_turn == BLACK) {
                rook_from_index += 56;
                rook_to_index += 56;
            }
            rook_bits = (C64(1) << rook_from_index) | (C64(1) << rook_to_index);
            Bitboards[current_turn] ^= rook_bits;
            Bitboards[Rooks] ^= rook_bits;

            // Update zobrist key
            z_key ^= piece_bitstrings[rook_from_index][current_turn][PIECE_ROOK - 2];
            z_key ^= piece_bitstrings[rook_to_index][current_turn][PIECE_ROOK - 2];

            piece_square_values_m[current_turn] -= lookup_ps_table_m(rook_from_index, PIECE_ROOK, current_turn);
            piece_square_values_e[current_turn] -= lookup_ps_table_e(rook_from_index, PIECE_ROOK, current_turn);

            piece_square_values_m[current_turn] += lookup_ps_table_m(rook_to_index, PIECE_ROOK, current_turn);
            piece_square_values_e[current_turn] += lookup_ps_table_e(rook_to_index, PIECE_ROOK, current_turn);
            break;
        }
        case MOVE_ENPASSANT: {
            U64 delete_square;
            int delete_index;
            if (current_turn == WHITE) {
                delete_index = move_to_index - 8;
                delete_square = C64(1) << (delete_index);
            } else {
                delete_index = move_to_index + 8;
                delete_square = C64(1) << (delete_index);
            }
            Bitboards[!current_turn] ^= delete_square;
            Bitboards[Pawns] ^= delete_square;

            // Take away the piece values for the side that pieces were captured
            piece_values[!current_turn] -= piece_to_value[PIECE_PAWN];

            // Update piece_count
//            piece_count[!current_turn][PIECE_PAWN - 2] -= 1;

            // Update zobrist key
            z_key ^= piece_bitstrings[delete_index][!current_turn][PIECE_PAWN - 2];

            // Take away piece square value for captured piece
            piece_square_values_m[!current_turn] -= lookup_ps_table_m(delete_index, PIECE_PAWN, !current_turn);
            piece_square_values_e[!current_turn] -= lookup_ps_table_e(delete_index, PIECE_PAWN, !current_turn);

            break;
        }
        case MOVE_PROMOTION: {
            Bitboards[Pawns] ^= to_bb;
            Bitboards[move.get_promote_to() + 3] ^= to_bb;

            // Change the piece_values score accordingly
            piece_values[current_turn] += piece_to_value[move.get_promote_to() + 3] - PAWN_VALUE;

            // Update piece_count
//            piece_count[current_turn][PIECE_PAWN - 2] -= 1;
//            piece_count[current_turn][move.get_promote_to() + 1] += 1; // Check move encoding to see why +1

            // Update zobrist key
            z_key ^= piece_bitstrings[move_to_index][current_turn][PIECE_PAWN - 2];
            z_key ^= piece_bitstrings[move_to_index][current_turn][move.get_promote_to() +
                                                                   1]; // Check move encoding to see why +1

            // Take away piece square value for captured piece
            piece_square_values_m[current_turn] -= lookup_ps_table_m(move_to_index, PIECE_PAWN, current_turn);
            piece_square_values_e[current_turn] -= lookup_ps_table_e(move_to_index, PIECE_PAWN, current_turn);

            piece_square_values_m[current_turn] += lookup_ps_table_m(move_to_index, move.get_promote_to() + 3,
                                                                     current_turn);
            piece_square_values_e[current_turn] += lookup_ps_table_e(move_to_index, move.get_promote_to() + 3,
                                                                     current_turn);
            break;
        }
    }

    // Set en_passant square
    // Clear the old en_passant change


    if (en_passant_square != -1) {
        U64 last_en_passant_file = rays[South][en_passant_square];
        z_key ^= en_passant_bitstrings[bitscan_forward(last_en_passant_file)];
    }

    if (move.get_piece_moved() == PIECE_PAWN && abs(move_from_index - move_to_index) == 16) {
        if (current_turn == WHITE) {
            en_passant_square = move_to_index - 8;
        } else {
            en_passant_square = move_to_index + 8;
        }

        // Update zobrist key:
        U64 en_passant_file = rays[South][en_passant_square];
        z_key ^= en_passant_bitstrings[bitscan_forward(en_passant_file)];
    } else {
        en_passant_square = -1;
    }

    // Check if pawn has moved
    // If so, reset halfmove counter

    if (move.get_piece_moved() == PIECE_PAWN) {
        halfmove_counter = -1;
    }

    // Check to see if castling is invalidated

    // see if rook was captured/moved
    if (move.get_piece_captured() != PIECE_NONE) {
        switch (move_to_index) {
            case 56:
                if (black_can_castle_queenside) {
                    black_can_castle_queenside = false;
                    z_key ^= black_castle_queenside_bitstring;
                }
                break;
            case 63:
                if (black_can_castle_kingside) {
                    black_can_castle_kingside = false;
                    z_key ^= black_castle_kingside_bitstring;
                }
                break;
            case 0:
                if (white_can_castle_queenside) {
                    white_can_castle_queenside = false;
                    z_key ^= white_castle_queenside_bitstring;
                }
                break;
            case 7:
                if (white_can_castle_kingside) {
                    white_can_castle_kingside = false;
                    z_key ^= white_castle_kingside_bitstring;
                }
                break;
        }
    }
    switch (move_from_index) {
        case 56:
            if (black_can_castle_queenside) {
                black_can_castle_queenside = false;
                z_key ^= black_castle_queenside_bitstring;
            }
            break;
        case 63:
            if (black_can_castle_kingside) {
                black_can_castle_kingside = false;
                z_key ^= black_castle_kingside_bitstring;
            }
            break;
        case 0:
            if (white_can_castle_queenside) {
                white_can_castle_queenside = false;
                z_key ^= white_castle_queenside_bitstring;
            }
            break;
        case 7:
            if (white_can_castle_kingside) {
                white_can_castle_kingside = false;
                z_key ^= white_castle_kingside_bitstring;
            }
            break;
    }


    // Check if king moves
    if (move.get_piece_moved() == PIECE_KING) {
        if (current_turn == BLACK) {
            if (black_can_castle_queenside) {
                black_can_castle_queenside = false;
                z_key ^= black_castle_queenside_bitstring;
            }
            if (black_can_castle_kingside) {
                black_can_castle_kingside = false;
                z_key ^= black_castle_kingside_bitstring;
            }
        } else {
            if (white_can_castle_queenside) {
                white_can_castle_queenside = false;
                z_key ^= white_castle_queenside_bitstring;
            }
            if (white_can_castle_kingside) {
                white_can_castle_kingside = false;
                z_key ^= white_castle_kingside_bitstring;
            }
        }
    }

    // Switch turns:

    z_key ^= black_to_move_bitstring;
    current_turn = !current_turn;

    // Increment halfmove counter
    halfmove_counter++;

}

void Board::unmake_move() {

    current_turn = !current_turn;

    move_data last_move = move_stack.back();
    Move move = last_move.move;
    assert(!last_move.is_null_move);

    white_can_castle_queenside = last_move.white_can_castle_queenside;
    white_can_castle_kingside = last_move.white_can_castle_kingside;
    black_can_castle_queenside = last_move.black_can_castle_queenside;
    black_can_castle_kingside = last_move.black_can_castle_kingside;

    en_passant_square = last_move.en_passant_square;

    z_key = last_move.z_key;

    halfmove_counter = last_move.halfmove_counter;


    // Actual act of making move:
    int move_from_index = move.get_from();
    int move_to_index = move.get_to();
    U64 from_bb = C64(1) << move_from_index;
    U64 to_bb = C64(1) << move_to_index;


    // Set back the captured squares first
    if (move.get_piece_captured() != PIECE_NONE && move.get_special_flag() != MOVE_ENPASSANT) {

        Bitboards[move.get_piece_captured()] ^= to_bb;
        Bitboards[!current_turn] ^= to_bb;

        // Add back the piece values for the captured piece
        piece_values[!current_turn] += piece_to_value[move.get_piece_captured()];

        // Update piece_count
//        piece_count[!current_turn][move.get_piece_captured() - 2] += 1;

        piece_square_values_m[!current_turn] += lookup_ps_table_m(move_to_index, move.get_piece_captured(),
                                                                  !current_turn);
        piece_square_values_e[!current_turn] += lookup_ps_table_e(move_to_index, move.get_piece_captured(),
                                                                  !current_turn);
    }



    // Flip the occupacy of the from square and to square
    Bitboards[current_turn] ^= from_bb | to_bb;
    Bitboards[move.get_piece_moved()] ^= from_bb | to_bb;

    piece_square_values_m[current_turn] += lookup_ps_table_m(move_from_index, move.get_piece_moved(), current_turn);
    piece_square_values_e[current_turn] += lookup_ps_table_e(move_from_index, move.get_piece_moved(), current_turn);

    piece_square_values_m[current_turn] -= lookup_ps_table_m(move_to_index, move.get_piece_moved(), current_turn);
    piece_square_values_e[current_turn] -= lookup_ps_table_e(move_to_index, move.get_piece_moved(), current_turn);



    // Special move handling now:

    switch (move.get_special_flag()) {
        case MOVE_NORMAL: {
            break;
        }
        case MOVE_CASTLING: {
            U64 rook_bits;
            int rook_from_index, rook_to_index;
            if (move.get_castle_type() == CASTLE_TYPE_KINGSIDE) {
                rook_from_index = 7;
                rook_to_index = 5;
            } else {
                rook_from_index = 0;
                rook_to_index = 3;
            }
            if (current_turn == BLACK) {
                rook_from_index += 56;
                rook_to_index += 56;
            }
            rook_bits = (C64(1) << rook_from_index) | (C64(1) << rook_to_index);
            Bitboards[current_turn] ^= rook_bits;
            Bitboards[Rooks] ^= rook_bits;

            piece_square_values_m[current_turn] += lookup_ps_table_m(rook_from_index, PIECE_ROOK, current_turn);
            piece_square_values_e[current_turn] += lookup_ps_table_e(rook_from_index, PIECE_ROOK, current_turn);

            piece_square_values_m[current_turn] -= lookup_ps_table_m(rook_to_index, PIECE_ROOK, current_turn);
            piece_square_values_e[current_turn] -= lookup_ps_table_e(rook_to_index, PIECE_ROOK, current_turn);

            break;
        }
        case MOVE_ENPASSANT: {
            U64 delete_square;
            int delete_index;
            if (current_turn == WHITE) {
                delete_index = move_to_index - 8;
                delete_square = C64(1) << (delete_index);
            } else {
                delete_index = move_to_index + 8;
                delete_square = C64(1) << (delete_index);
            }
            Bitboards[!current_turn] ^= delete_square;
            Bitboards[Pawns] ^= delete_square;

            // Take away the piece values for the side that pieces were captured
            piece_values[!current_turn] += piece_to_value[PIECE_PAWN];

            // Update piece_count
//            piece_count[!current_turn][PIECE_PAWN - 2] += 1;

            // Take away piece square value for captured piece
            piece_square_values_m[!current_turn] += lookup_ps_table_m(delete_index, PIECE_PAWN, !current_turn);
            piece_square_values_e[!current_turn] += lookup_ps_table_e(delete_index, PIECE_PAWN, !current_turn);
            break;
        }
        case MOVE_PROMOTION: {
            Bitboards[Pawns] ^= to_bb;
            Bitboards[move.get_promote_to() + 3] ^= to_bb;

            // Change the piece_values score accordingly
            piece_values[current_turn] -= piece_to_value[move.get_promote_to() + 3] - PAWN_VALUE;

            // Update piece_count
//            piece_count[current_turn][PIECE_PAWN - 2] += 1;
//            piece_count[current_turn][move.get_promote_to() + 1] -= 1;

            // Take away piece square value for captured piece
            piece_square_values_m[current_turn] += lookup_ps_table_m(move_to_index, PIECE_PAWN, current_turn);
            piece_square_values_e[current_turn] += lookup_ps_table_e(move_to_index, PIECE_PAWN, current_turn);

            piece_square_values_m[current_turn] -= lookup_ps_table_m(move_to_index, move.get_promote_to() + 3,
                                                                     current_turn);
            piece_square_values_e[current_turn] -= lookup_ps_table_e(move_to_index, move.get_promote_to() + 3,
                                                                     current_turn);
            break;
        }
    }

    move_stack.pop_back();
}


void Board::make_null_move() {
    move_data m;
    m.en_passant_square = en_passant_square;
    m.is_null_move = true;
    m.z_key = z_key;
    m.halfmove_counter = halfmove_counter;
    move_stack.push_back(m);

    // Reset en_passant_square and clear it from z_key
    if (en_passant_square != -1) {
        U64 last_en_passant_file = rays[South][en_passant_square];
        z_key ^= en_passant_bitstrings[bitscan_forward(last_en_passant_file)];
    }
    en_passant_square = -1;

    // Swap turn
    z_key ^= black_to_move_bitstring;
    current_turn = !current_turn;

    // Should halfmove_counter be updated?
    halfmove_counter = 0;
}

void Board::unmake_null_move() {

    current_turn = !current_turn;

    move_data last_move = move_stack.back();
    assert(last_move.is_null_move);
    z_key = last_move.z_key;
    en_passant_square = last_move.en_passant_square;


    move_stack.pop_back();

    // Should halfmove_counter be updated?
    halfmove_counter = last_move.halfmove_counter;
}


// MOVE GENERATION END


// MOVE ORDERING BEGIN

void Board::assign_move_scores(MoveList& moves, HashMove hash_move) {
    unsigned int score;

    // Score all the moves
    for (auto it = moves.begin(); it != moves.end(); ++it) {
        score = 512;
        if (it->is_capture()) {
            score += 70;
            score += piece_to_value_small[it->get_piece_captured()] - piece_to_value_small[it->get_piece_moved()];
        }
        if (it->get_special_flag() == MOVE_PROMOTION) {
            score += 100;
        } else if (it->get_special_flag() == MOVE_CASTLING) {
            score += 60;
        }

        // Placing piece at square attacked by pawn is stupid, so subtract from score if that happens
        /*
         if (pawn_attacks[current_turn][it->get_to()] & Bitboards[Pawns] & Bitboards[!current_turn]) {
         score -= piece_to_value_small[it->get_piece_moved()];
         }
         */

        if (hash_move == (*it)) {
            score = 1000;
        }

        it->set_move_score(score);
    }
}

void Board::sort_moves(MoveList& moves) {
    // Deprecated
    assign_move_scores(moves, HashMove());

    std::sort(moves.begin(), moves.end(), move_cmp);
}


// MOVE ORDERING END


// EVALUATION BEGIN

void Board::calculate_piece_values() {
    // Should only be called during init; piece values updated incrementally
    U64 white_pieces = Bitboards[WhitePieces];
    U64 black_pieces = Bitboards[BlackPieces];
    piece_values[WhitePieces] = PAWN_VALUE * pop_count(Bitboards[Pawns] & white_pieces)
                                + KNIGHT_VALUE * pop_count(Bitboards[Knights] & white_pieces)
                                + BISHOP_VALUE * pop_count(Bitboards[Bishops] & white_pieces)
                                + ROOK_VALUE * pop_count(Bitboards[Rooks] & white_pieces)
                                + QUEEN_VALUE * pop_count(Bitboards[Queens] & white_pieces);
    piece_values[BlackPieces] = PAWN_VALUE * pop_count(Bitboards[Pawns] & black_pieces)
                                + KNIGHT_VALUE * pop_count(Bitboards[Knights] & black_pieces)
                                + BISHOP_VALUE * pop_count(Bitboards[Bishops] & black_pieces)
                                + ROOK_VALUE * pop_count(Bitboards[Rooks] & black_pieces)
                                + QUEEN_VALUE * pop_count(Bitboards[Queens] & black_pieces);
}

void Board::print_piece_values() {
    std::cout << "\nWhite Piece Values: " << piece_values[WhitePieces];
    std::cout << "\nBlack Piece Values: " << piece_values[BlackPieces] << '\n';
}

void Board::calculate_piece_square_values() {
    piece_square_values_m[WHITE] = 0;
    piece_square_values_e[WHITE] = 0;
    piece_square_values_m[BLACK] = 0;
    piece_square_values_e[BLACK] = 0;

    U64 w_p = Bitboards[WhitePieces];
    U64 b_p = Bitboards[BlackPieces];


    // Iterate through pieces and calculate piece_square_values
    for (int i = Kings; i <= Pawns; i++) {
        U64 pieces = Bitboards[i] & w_p;

        if (pieces)
            do {
                int index = bitscan_forward(pieces);
                piece_square_values_m[WHITE] += lookup_ps_table_m(index, i, WHITE);
                piece_square_values_e[WHITE] += lookup_ps_table_e(index, i, WHITE);
            } while (pieces &= pieces - 1);
    }

    for (int i = Kings; i <= Pawns; i++) {
        U64 pieces = Bitboards[i] & b_p;

        if (pieces)
            do {
                int index = bitscan_forward(pieces);
                piece_square_values_m[BLACK] += lookup_ps_table_m(index, i, BLACK);
                piece_square_values_e[BLACK] += lookup_ps_table_e(index, i, BLACK);
            } while (pieces &= pieces - 1);
    }
}

void Board::print_piece_square_values() {
    std::cout << "White midgame ps values: " << piece_square_values_m[WHITE] << "\nWhite endgame ps values: "
              << piece_square_values_e[WHITE] << "\nBlack midgame ps values: " << piece_square_values_m[BLACK]
              << "\nBlack endgame ps values: " << piece_square_values_e[BLACK] << '\n';
}

int Board::calculate_game_phase() {
    int game_phase = 0;
    game_phase += pop_count(Bitboards[Knights] | Bitboards[Bishops]);
    game_phase += pop_count(Bitboards[Rooks]) * 2;
    game_phase += pop_count(Bitboards[Queens]) * 4;
    // Handle early promotion
    game_phase = std::min(game_phase, 24);
    return game_phase;
}

int Board::calculate_pawn_shield_bonus() {
    int score = 0;
    if (king_loc_kingside_castled[WHITE] & Bitboards[Kings] & Bitboards[WHITE]) {
        // Pawn shield on second rank is better than on third rank
        score += pop_count(Bitboards[WHITE] & Bitboards[Pawns] & pawn_shield_kingside[WHITE] & second_or_seventh_rank) *
                 32;
        score +=
                pop_count(Bitboards[WHITE] & Bitboards[Pawns] & pawn_shield_kingside[WHITE] & third_or_sixth_rank) * 16;
    } else if (king_loc_queenside_castled[WHITE] & Bitboards[Kings] & Bitboards[WHITE]) {
        // Pawn shield on second rank is better than on third rank
        score +=
                pop_count(Bitboards[WHITE] & Bitboards[Pawns] & pawn_shield_queenside[WHITE] & second_or_seventh_rank) *
                32;
        score += pop_count(Bitboards[WHITE] & Bitboards[Pawns] & pawn_shield_queenside[WHITE] & third_or_sixth_rank) *
                 16;
    }

    if (king_loc_kingside_castled[BLACK] & Bitboards[Kings] & Bitboards[BLACK]) {
        // Pawn shield on seventh rank is better than on sixth rank
        score -= pop_count(Bitboards[BLACK] & Bitboards[Pawns] & pawn_shield_kingside[BLACK] & second_or_seventh_rank) *
                 32;
        score -=
                pop_count(Bitboards[BLACK] & Bitboards[Pawns] & pawn_shield_kingside[BLACK] & third_or_sixth_rank) * 16;
    } else if (king_loc_queenside_castled[BLACK] & Bitboards[Kings] & Bitboards[BLACK]) {
        // Pawn shield on seventh rank is better than on sixth rank
        score -=
                pop_count(Bitboards[BLACK] & Bitboards[Pawns] & pawn_shield_queenside[BLACK] & second_or_seventh_rank) *
                32;
        score -= pop_count(Bitboards[BLACK] & Bitboards[Pawns] & pawn_shield_queenside[BLACK] & third_or_sixth_rank) *
                 16;
    }
    return score;
}

int Board::static_eval() {
    // Returns eval in terms of side to play
    int eval = 0;

    eval += piece_values[WHITE] - piece_values[BLACK];

    int midgame_score = 0;
    midgame_score += piece_square_values_m[WHITE] - piece_square_values_m[BLACK];
    int endgame_score = 0;
    endgame_score += piece_square_values_e[WHITE] - piece_square_values_e[BLACK];


    // Pawn shield in front of king
    midgame_score += calculate_pawn_shield_bonus();


    // Calculate game phase:
    int game_phase = calculate_game_phase();

    eval += ((midgame_score * game_phase) + (endgame_score * (24 - game_phase))) / 24;

    // Consider mobility:
    eval += calculate_mobility() * MOBILITY_WEIGHT;

    eval *= current_turn == 0 ? 1 : -1;
    return eval;
}

bool Board::has_repeated_once() {
    // Checks for repetition
    // TODO: optimize by only comparing current_z_key every 2 plies

    U64 current_z_key = get_z_key();

    for (auto it = move_stack.rbegin(); it != move_stack.rend(); ++it) {
        if (it->z_key == current_z_key) {
            return true;
        }
        if (it->move.is_capture() || it->move.get_special_flag() != MOVE_NORMAL ||
            it->move.get_piece_moved() == PIECE_PAWN ||
            it->white_can_castle_kingside != white_can_castle_kingside ||
            it->white_can_castle_queenside != white_can_castle_queenside ||
            it->black_can_castle_kingside != black_can_castle_kingside ||
            it->black_can_castle_queenside != black_can_castle_queenside ||
            it->is_null_move) {
            return false;
        }
    }
    return false;
}

bool Board::has_repeated_twice() {
    // Checks for threefold repetition
    // Can optimize by only comparing current_z_key every 2 plies instead, but this function shouldn't be used in perfomance critical areas


    bool first_rep_flag = false;
    U64 current_z_key = get_z_key();

    for (auto it = move_stack.rbegin(); it != move_stack.rend(); ++it) {
        if (it->z_key == current_z_key) {
            if (first_rep_flag) {
                return true;
            }
            first_rep_flag = true;
        }
        if (it->move.is_capture() || it->move.get_special_flag() != MOVE_NORMAL ||
            it->move.get_piece_moved() == PIECE_PAWN ||
            it->white_can_castle_kingside != white_can_castle_kingside ||
            it->white_can_castle_queenside != white_can_castle_queenside ||
            it->black_can_castle_kingside != black_can_castle_kingside ||
            it->black_can_castle_queenside != black_can_castle_queenside ||
            it->is_null_move) {
            return false;
        }
    }
    return false;
}

bool Board::has_drawn_by_fifty_move_rule() {
    // Please check for checkmate before this function is called
    return halfmove_counter >= 100;
}


// USER INTERFACE BEGIN:
// These are implementation specific functions

Move Board::request_move(Move move) {
    MoveList moves;
    generate_moves(moves);

    for (auto it = moves.begin(); it != moves.end(); ++it) {
        if (it->first_twelfth_eq(move)) {
            // Check if promotion lines up:
            if (it->get_special_flag() == MOVE_PROMOTION) {
                if (it->get_promote_to() == move.get_promote_to()) {
                    make_move(*it);
                    return *it;
                }
                continue;
            } else {
                make_move(*it);
                return *it;
            }
        }
    }
    move.set_as_illegal();
    return move;
}

bool Board::is_trying_to_promote(Move move) {
    if (find_piece_occupying_sq(move.get_from()) != PIECE_PAWN) {
        return false;
    }
    MoveList moves;
    generate_moves(moves);

    if (current_turn == WHITE) {
        for (auto it = moves.begin(); it != moves.end(); ++it) {
            if (it->first_twelfth_eq(move) && move.get_to() >= 56) {
                return true;
            }
        }
    } else {
        for (auto it = moves.begin(); it != moves.end(); ++it) {
            if (it->first_twelfth_eq(move) && move.get_to() <= 7) {
                return true;
            }
        }
    }
    return false;
}


U64 Board::get_z_key() {
    return z_key;
}

std::vector<move_data> Board::get_move_stack() {
    return move_stack;
}

bool Board::get_reg_starting_pos() {
    return reg_starting_pos;
}
