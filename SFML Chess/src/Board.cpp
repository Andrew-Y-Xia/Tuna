//
//  Board.cpp
//  SFML Chess
//
//  Created by Andrew Xia on 4/16/21.
//  Copyright © 2021 Andy. All rights reserved.
//
#include "depend.hpp"
#include "Utility.hpp"
#include "Board.hpp"

extern sf::Texture textures[13];
extern std::forward_list<sf::Sprite> sprites, promotion_sprites_white, promotion_sprites_black;
extern int incre8[8];

Board::Board() {
    standard_setup();
    read_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    post_readLEN();
}

void Board::find_kings() {
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (squares[y][x].piece == King) {
                if (squares[y][x].color == 1) {
                    black_king_loc = Cords{x, y};
                }
                else {
                    white_king_loc = Cords{x, y};
                }
            }
        }
    }
}

void Board::standard_setup() {
    Cords n = {-1, -1};
    Cords m = {-2, -2};
    pinned_by.set_empty_key(n);
    pinning.set_empty_key(n);

    pinned_by.set_deleted_key(m);
    pinning.set_deleted_key(m);

    previous_board_positions.set_empty_key(std::string());
}

void Board::post_readLEN() {
    find_kings();
    generate_pins(0, incre8);
    generate_pins(1, incre8);
//        generate_moves(legal_moves);
    reset_piece_values();
}

Board::Board(std::string str) {
    standard_setup();
    read_FEN(str);
    post_readLEN();
}

int Board::get_current_turn() {
    return current_turn;
}

int Board::piece_to_value(piece_type piece) {
    switch (piece) {
        case Empty:
        case King:
            return 0;
        case Pawn:
            return PAWN_VALUE;
        case Knight:
            return KNIGHT_VALUE;
        case Bishop:
            return BISHOP_VALUE;
        case Rook:
            return ROOK_VALUE;
        case Queen:
            return QUEEN_VALUE;
    }
}

void Board::reset_piece_values() {
    white_piece_values = 0;
    black_piece_values = 0;


    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            if (squares[y][x].color == 1) {
                black_piece_values += piece_to_value(squares[y][x].piece);
            }
            else {
                white_piece_values += piece_to_value(squares[y][x].piece);
            }
        }
    }
}

void Board::set_texture_to_pieces() {
    Square current_square;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            current_square = squares[y][x];

            if (current_square.piece != Empty) {

                sf::Sprite sprite;

                set_single_texture(current_square.color, current_square.piece, sprite);


                sprite.setOrigin(sf::Vector2f(30, 30));
                sprite.setPosition(x * WIDTH/8 + WIDTH/16 - OFFSET, y * WIDTH/8 + WIDTH/16 - OFFSET);
                sprite.setScale(sf::Vector2f(SCALE, SCALE));
                sprites.push_front(sprite);

            }
//                std::cout << x << ", " << y << '\n';
        }
    }
}


std::string Board::generate_FEN() {
    std::string str;
    int running_counter = 0;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            char c = piece_type_to_char(squares[y][x].piece);
            if (squares[y][x].color == 0 && squares[y][x].piece != Empty) {
                c = (char) toupper(c);
            }

            if (c == '0') {
                running_counter += 1;
            }
            else if (running_counter != 0) {
                str.append(std::to_string(running_counter));
                running_counter = 0;
            }
            if (c != '0') {
                str.append(1, c);
            }
        }
        if (running_counter != 0) {
            str.append(std::to_string(running_counter));
            running_counter = 0;
        }
        if (y != 7) {
            str.append(1, '/');
        }
    }
    str.append(1, ' ');
    if (current_turn == 1) {
        str.append(1, 'b');
    }
    else {
        str.append(1, 'w');
    }
    str.append(1, ' ');
    if (white_can_castle_kingside || white_can_castle_queenside || black_can_castle_kingside || black_can_castle_queenside) {
        if (white_can_castle_kingside) {
            str.append(1, 'K');
        }
        if (white_can_castle_queenside) {
            str.append(1, 'Q');
        }
        if (black_can_castle_kingside) {
            str.append(1, 'k');
        }
        if (black_can_castle_queenside) {
            str.append(1, 'q');
        }
    }
    else {
        str.append(1, '-');
    }
    str.append(1, ' ');

    if (en_passant_cords == Cords{-1, -1}) {
        str.append(1, '-');
    }
    else {
        char c = num_to_char(en_passant_cords.x);
        str.append(1, c);
        str.append(std::to_string(8 - en_passant_cords.y));
    }

    str.append(1, ' ');
    str.append(std::to_string(halfmove_counter));
    str.append(1, ' ');
    str.append(std::to_string(fullmove_counter));

    return str;
}


void Board::read_FEN(std::string str) {
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
                    for (int i = 0; i < blanks; i++) {
                        Square square;
                        square.color = 0;
                        square.piece = Empty;
                        squares[y][x] = square;
                        x++;
                    }
                }
                else {
                    Square square;
                    if (isupper(*it)) {
                        square.color = 0;
                    }
                    else {
                        square.color = 1;
                    }

                    switch ((char) tolower(*it)) {
                        case 'r':
                            square.piece = Rook;
                            break;
                        case 'b':
                            square.piece = Bishop;
                            break;
                        case 'n':
                            square.piece = Knight;
                            break;
                        case 'k':
                            square.piece = King;
                            break;
                        case 'q':
                            square.piece = Queen;
                            break;
                        case 'p':
                            square.piece = Pawn;
                            break;
                        default:
                            std::cout << "This should not have been reached. Invalid piece: " << (char) tolower(*it) <<'\n';
                    }
                    squares[y][x] = square;
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

        en_passant_cords.x = x;
        en_passant_cords.y = y;
    }
    else {
        en_passant_cords.x = -1;
        en_passant_cords.y = -1;
    }
}

char Board::piece_type_to_char(piece_type p) {
    switch (p) {
        case Empty:
            return '0';
        case Pawn:
            return 'p';
        case Knight:
            return 'n';
        case Bishop:
            return 'b';
        case Rook:
            return 'r';
        case Queen:
            return 'q';
        case King:
            return 'k';
    }
}

void Board::debug_print() {
    for (int i = 0; i < 30; i++) {
        std::cout << std::endl;
    }
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            char c = piece_type_to_char(squares[y][x].piece);

            if (squares[y][x].color == 0 && squares[y][x].piece != Empty) {
                c = (char) toupper(c);
            }
            std::cout << c << ' ';
        }
        std::cout << std::endl;
    }

    std::cout << std::endl;
    std::cout << "White can castle queenside: " << white_can_castle_queenside << std::endl;
    std::cout << "White can castle kingside: " << white_can_castle_kingside << std::endl;
    std::cout << "Black can castle queenside: " << black_can_castle_queenside << std::endl;
    std::cout << "Black can castle kingside: " << black_can_castle_kingside << std::endl;
    std::cout << std::endl;
    std::cout << "Enpassant cords: " << en_passant_cords.x << ' ' << en_passant_cords.y << std::endl;
}

Cords Board::sliding_pieces_incrementer(int x, int y, int increment_x, int increment_y, bool ignore_king) {
    Cords c = {x, y};

    while (1) {
        c.x += increment_x;
        c.y += increment_y;


        if (!is_within_bounds(c.x, c.y)) {
            c.x -= increment_x;
            c.y -= increment_y;
            return c;
        }
        else if (squares[c.y][c.x].piece != Empty) {
            if (!(ignore_king && squares[c.y][c.x].piece == King && current_turn == squares[c.y][c.x].color)) {
                return c;
            }
        }
    }
}

Cords Board::ignore_square_incrementer(int x, int y, int increment_x, int increment_y, Cords ignore_squares[2]) {
    Cords c = {x, y};
    while (1) {
        c.x += increment_x;
        c.y += increment_y;


        if (!is_within_bounds(c.x, c.y)) {
            c.x -= increment_x;
            c.y -= increment_y;
            return c;
        }
        else if (squares[c.y][c.x].piece != Empty) {
            if (!(c == ignore_squares[0] || c == ignore_squares[1])) {
                return c;
            }
        }
    }
}

void Board::debug_attacked_squares(int attacker_color) {
    std::cout << '\n' << '\n' << '\n' << '\n';
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
//                is_square_under_attack(x, y, attacker_color);
            std::cout << is_square_under_attack(x, y, attacker_color) << ' ';
        }
        std::cout << std::endl;
    }
}

// Lots of boilerplate here
void Board::pin_slider(int x, int y, int increment_x, int increment_y, Cords& pinned_piece, Cords& pinning_piece ) {
    Cords c = {x, y};
    bool found_possible_pinned_piece = false;

    while (1) {
        c.x += increment_x;
        c.y += increment_y;


        if (!is_within_bounds(c.x, c.y)) {
            pinned_piece = Cords{-1, -1};
            pinning_piece = Cords{-1, -1};
            return;
        }
        // Test to see if piece is reached
        else if (squares[c.y][c.x].piece != Empty) {
            // First see if a possible pinned piece has not been found
            if (!found_possible_pinned_piece) {
                // If it is the same color, piece may be pinned!
                if (squares[y][x].color == squares[c.y][c.x].color) {
                    pinned_piece = c;
                    found_possible_pinned_piece = true;
                    continue;
                }
                // If it's not the same color, there can't be a pin no matter what
                else {
                    pinned_piece = Cords{-1, -1};
                    pinning_piece = Cords{-1, -1};
                    return;
                }
            }
            // If a possible pinned piece has already been found
            else {
                // First check to see if the piece is same color. If it is, there cannot be a pin.
                if (squares[y][x].color == squares[c.y][c.x].color) {
                    pinned_piece = Cords{-1, -1};
                    pinning_piece = Cords{-1, -1};
                    return;
                }
                // If the piece is of opposite color, check to see if it has the ability to pin (i.e bishops diagonal, rooks vert/hori)
                switch (squares[c.y][c.x].piece) {
                    // Queen goes first since rook + bishop patterns = queen patterns
                    case Queen:
                        if (x == c.x || y == c.y || abs(c.y - y) == abs(c.x - x)) {
                            pinning_piece = c;
                            return;
                        }
                        else {
                            pinned_piece = Cords{-1, -1};
                            pinning_piece = Cords{-1, -1};
                            return;
                        }
                    case Bishop:
                        if (abs(c.y - y) == abs(c.x - x)) {
                            pinning_piece = c;
                            return;
                        }
                        else {
                            pinned_piece = Cords{-1, -1};
                            pinning_piece = Cords{-1, -1};
                            return;
                        }
                    case Rook:
                        if (x == c.x || y == c.y) {
                            pinning_piece = c;
                            return;
                        }
                        else {
                            pinned_piece = Cords{-1, -1};
                            pinning_piece = Cords{-1, -1};
                            return;
                        }
                    // Non slider pieces have no ability to pin
                    default:
                        pinned_piece = Cords{-1, -1};
                        pinning_piece = Cords{-1, -1};
                        return;
                }
            }
        }
    }
}

void Board::debug_pins() {
    for (auto it = pinning.begin(); it != pinning.end(); ++it) {
        std::cout << it->first.x << ' ' << it->first.y << " is pinning ";
        std::cout << it->second.x << ' ' << it->second.y << '\n';
    }
    std::cout << '\n';
    for (auto it = pinned_by.begin(); it != pinned_by.end(); ++it) {
        std::cout << it->first.x << ' ' << it->first.y << " is pinned by ";
        std::cout << it->second.x << ' ' << it->second.y << '\n';
    }
    std::cout << '\n' << '\n';
}


void Board::delete_pins(int color, int* increments) {
    // Dont pass anything other than array size 8 to increments!!! Otherwise segfault
    for (auto it = pinning.begin(); it != pinning.end(); ++it) {
        // If the piece pinning is of opposite color, delete the pin data (deletion is neccesary otherwise iterator will iterate over it next time)
        // Also, look up the piece being pinned and delete this cord from the list

        if (squares[it->first.y][it->first.x].color != color && *(increments + increment_to_index(Cords{sgn(it->first.x - it->second.x), sgn(it->first.y - it->second.y)}))) {
            pinned_by.erase(it->second);
            /*
             if (pinned_by[*(it->second.begin())].empty()) {
             pinned_by.erase(*(it->second.begin()));
             }
             */
            pinning.erase(it->first);
        }
    }
}

void Board::generate_pins(int color, int* increments) {
    // Dont pass anything other than array size 8 to increments!!! Otherwise segfault
    // Color
    Cords king, pinned_piece, pinning_piece;

    king = color == 1 ? black_king_loc : white_king_loc;


    const int size = 8;
    Cords c;
    for (int i = 0; i < size; i++) {
        if (*(increments + i)) {
            c = index_to_increment(i);
            pin_slider(king.x, king.y, c.x, c.y, pinned_piece, pinning_piece);
            if (pinned_piece.x != -1) {
//                    std::cout << "Found pinned piece: " << pinned_piece.x << ' ' << pinned_piece.y << " | pinner: " << pinning_piece.x << ' ' << pinning_piece.y <<  '\n';
                pinned_by[Cords{pinned_piece.x, pinned_piece.y}] = Cords{pinning_piece.x, pinning_piece.y};
                pinning[Cords{pinning_piece.x, pinning_piece.y}] = Cords{pinned_piece.x, pinned_piece.y};
            }
        }
    }

//        debug_pins();
}

bool Board::is_in_between(Cords c1, Cords c2, Cords move_to) {
    return (std::min(c1.x, c2.x) <= move_to.x && move_to.x <= std::max(c1.x, c2.x) && std::min(c1.y, c2.y) <= move_to.y && move_to.y <= std::max(c1.y, c2.y));
}

void Board::print_attacks_on_king() {
    for (auto it = attacks_on_the_king.begin(); it != attacks_on_the_king.end(); ++it) {
        std::cout << it->x << ", " << it->y << std::endl;
    }
//        std::cout << white_king_loc.x << white_king_loc.y;
}


bool Board::follows_check_rules(Move move) {

    Cords king_c = current_turn == 1 ? black_king_loc : white_king_loc;
    // If the king is being moved, then there's no need to do any checks cause the king's individual checks will make sure no illegal moves
    if (king_c == move.from_c) {
        return true;
    }

    // Check if in check
    if (!attacks_on_the_king.empty()) {
        // Iterate through the squares that are attacking king
        for (auto attacking_piece = attacks_on_the_king.begin(); attacking_piece != attacks_on_the_king.end(); ++attacking_piece) {
            if (move.type == En_Passant && move.from_c.y == attacking_piece->y && move.to_c.x == attacking_piece->x) {
                continue;
            }
            if (!is_in_between(*attacking_piece, king_c, move.to_c)) {
                return false;
            }
            // If slider piece, see if block-able
            switch (squares[attacking_piece->y][attacking_piece->x].piece) {
                case Queen:
                    if (abs(move.to_c.y - attacking_piece->y) == abs(move.to_c.x - attacking_piece->x)) {
                        continue;
                    }
                    else if (attacking_piece->x == move.to_c.x && king_c.x == move.to_c.x) {
                        continue;
                    }
                    else if (attacking_piece->y == move.to_c.y && king_c.y == move.to_c.y) {
                        continue;
                    }
                    else {
                        return false;
                    }
                    break;
                case Bishop:
                    if (abs(move.to_c.y - attacking_piece->y) != abs(move.to_c.x - attacking_piece->x)) {
                        return false;
                    }
                    break;
                case Rook:
                    if (!(attacking_piece->x == move.to_c.x || attacking_piece->y == move.to_c.y)) {
                        return false;
                    }
                    break;
                // If normal piece, see if taking the attacking piece
                case Pawn:
                case Knight:
                case King:
                    if (move.to_c != *attacking_piece) {
                        return false;
                    }
                    break;
                default:
                    std::cout << "should not have been reached follows_check_rules";
            }
        }
    }
    return true;
}

inline bool Board::does_pass_basic_piece_checks(Move move) {
    if (!(is_correct_turn(move.from_c.x, move.from_c.y))) {
        return false;
    }
    else if (!(is_within_bounds(move.to_c.x, move.to_c.y))) {
        return false;
    }
    // Equality cords check
    else if (move.from_c.x == move.to_c.x && move.from_c.y == move.to_c.y){
        return false;
    }
    else if (is_friendly_piece(move.to_c.x, move.to_c.y)){
        return false;
    }
    else if (!follows_pin_rules(move)) {
        return false;
    }
    else if (!follows_check_rules(move)) {
        return false;
    }
    else {
        return true;
    }
}

void Board::reset_attacks_on_the_king() {
    attacks_on_the_king.clear();

    Cords king_c = current_turn == 1 ? black_king_loc : white_king_loc;
    attacks_on_the_king = under_attack_cords(king_c.x, king_c.y, !current_turn);
}

int Board::generate_moves(std::vector<Move>& moves) {
    int moves_counter = 0;
    moves.clear();
    reset_attacks_on_the_king();


    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {

            if (squares[y][x].color == current_turn) {
                switch (squares[y][x].piece) {

                    case Empty:
                        break;
                    case Pawn:
                        moves_counter += generate_pawn_moves(moves, x, y);
                        break;
                    case Knight:
                        moves_counter += generate_knight_moves(moves, x, y);
                        break;
                    case Bishop:
                        moves_counter += generate_bishop_moves(moves, x, y);
                        break;
                    case Rook:
                        moves_counter += generate_rook_moves(moves, x, y);
                        break;
                    case Queen:
                        moves_counter += generate_queen_moves(moves, x, y);
                        break;
                    case King:
                        moves_counter += generate_king_moves(moves, x, y);
                        break;
                    default:
                        std::cout << "Should not have been reached at generate_moves." << std::endl;
                }
            }
        }
    }
//        debug_print_moves(moves);
    return moves_counter;
}

inline bool Board::calculate_en_passant_pins(const Cords &king_c, int x, int y, int incr_x, int incr_y) {

    Cords ignore_squares[2] = {Cords{x, y}, Cords{en_passant_cords.x, y}};
    Cords c = ignore_square_incrementer(king_c.x, king_c.y, incr_x, incr_y, ignore_squares);
    if (king_c.x == x && king_c.y == y) {
        std::cout << "Shouldn't occur calculate_en_passant_pins";
    }
    if (squares[c.y][c.x].color != current_turn) {
        switch (squares[c.y][c.x].piece) {
            case Empty:
            case Pawn:
            case Knight:
            case King:
                break;
            case Queen:
                if (c.y == king_c.y || abs(king_c.y - c.y) == abs(king_c.x - c.x)) {
                    return true;
                }
                break;
            case Bishop:
                if (abs(king_c.y - c.y) == abs(king_c.x - c.x)) {
                    return true;
                }
                break;
            case Rook:
                if (c.y == king_c.y) {
                    return true;
                }
                break;
            default:
                std::cout << "Should not be reached under_attack_cords";
        }
    }
    return false;
}

inline int Board::generate_pawn_moves(std::vector<Move>& moves, int x, int y, bool ignore_turns) {
    int moves_counter = 0;

    int direction_value, pawn_start_y, opposite_side_value;
    Cords orig = Cords {x, y};
    Move move = {orig, orig, Normal};

    // Switch some increment values depending on side
    if (squares[y][x].color == 1) {
        direction_value = 1;
        pawn_start_y = 1;
    }
    else {
        direction_value = -1;
        pawn_start_y = 6;
    }

    // Check for going straight ahead
    if (squares[y + direction_value][x].piece == Empty) {
        move.to_c.y += direction_value;
        moves_counter += pawn_path_handle_push_move(moves, move, ignore_turns);
        move.to_c = orig;
        // Check for double length for first pawn move
        if (y == pawn_start_y && squares[y + direction_value * 2][x].piece == Empty) {
            move.to_c.y += direction_value * 2;
            moves_counter += pawn_path_handle_push_move(moves, move, ignore_turns);
            move.to_c = orig;
        }
    }

    Cords king_c = current_turn == 1 ? black_king_loc : white_king_loc;
    int side_value = current_turn == 1 ? 4 : 3;

    bool need_to_calculate_pins = abs(en_passant_cords.x - king_c.x) == abs(side_value - king_c.y) || en_passant_cords.x == king_c.x || side_value == king_c.y;


    // See if Pawn can capture two diagonal squares
    for (int i = -1; i < 3; i+=2) {
        if (is_within_bounds(x + i, y + direction_value)) {
            if (squares[y + direction_value][x + i].piece != Empty) {
                move.to_c.y += direction_value;
                move.to_c.x += i;
                moves_counter += pawn_path_handle_push_move(moves, move, ignore_turns);
                move.to_c = orig;
            }
            // See if pawn can capture through en passant
            else if (en_passant_cords.y == y + direction_value && en_passant_cords.x == x + i) {
                bool en_passant_is_pinned = false;
                if (need_to_calculate_pins) {
                    // Check to see making an en_passant will result in exposing an attack on the king
                    int incr_x = sgn(en_passant_cords.x - king_c.x);
                    int incr_y = sgn(side_value - king_c.y);
                    en_passant_is_pinned = calculate_en_passant_pins(king_c, x, y, incr_x, incr_y);
                }
                if (!en_passant_is_pinned) {
                    move.to_c.y += direction_value;
                    move.to_c.x += i;
                    move.type = En_Passant;
                    moves_counter += pawn_path_handle_push_move(moves, move, ignore_turns);
                    move.to_c = orig;
                    move.type = Normal;
                }
            }
        }
    }
    return moves_counter;
}

inline int Board::pawn_path_handle_push_move(std::vector<Move>& moves, Move& move, bool ignore_turns) {
    if (!does_pass_basic_piece_checks(move)) {
        return 0;
    }

    int opposite_side_value;
    if (squares[move.from_c.y][move.from_c.x].color == 1) {
        opposite_side_value = 7;
    }
    else {
        opposite_side_value = 0;
    }
    if (move.to_c.y == opposite_side_value) {
        move.type = Promote_to_Queen;
        moves.push_back(move);
        move.type = Promote_to_Rook;
        moves.push_back(move);
        move.type = Promote_to_Bishop;
        moves.push_back(move);
        move.type = Promote_to_Knight;
        moves.push_back(move);
        move.type = Normal;
        return 4;
    }
    else {
        moves.push_back(move);
        return 1;
    }
    return 0;
}

inline int Board::reg_piece_handle_push_move(std::vector<Move>& moves, Move& move) {
    if (!does_pass_basic_piece_checks(move)) {
        return 0;
    }
    else {
        moves.push_back(move);
        return 1;
    }
}

inline int Board::generate_king_moves(std::vector<Move>& moves, int x, int y, bool ignore_turns) {
    int moves_counter = 0;

    if (!(is_correct_turn(x, y)) && !ignore_turns) {
        return 0;
    }

    Cords orig = Cords {x, y};
    Move move = {orig, orig, Normal};

    int home_side_value = current_turn == 1 ? 0 : 7;

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (i == 0 && j == 0) {
                continue;
            }
            else if (!is_within_bounds(x + j, y + i)) {
                continue;
            }
            else if (!is_square_under_attack(x + j, y + i, !current_turn)){
                move.to_c = Cords{x + j, y + i};
                moves_counter += reg_piece_handle_push_move(moves, move);
                move.to_c = orig;
            }
        }
    }

    int can_castle_queenside = current_turn ? black_can_castle_queenside : white_can_castle_queenside;
    int can_castle_kingside = current_turn ? black_can_castle_kingside : white_can_castle_kingside;

    // Logic here -> yikes
    if (can_castle_queenside && squares[home_side_value][3].piece == Empty && squares[home_side_value][2].piece == Empty && squares[home_side_value][1].piece == Empty && !is_square_under_attack(move.from_c.x, move.from_c.y, !current_turn) && !is_square_under_attack(3, home_side_value, !current_turn) && !is_square_under_attack(2, home_side_value, !current_turn)) {
        move.to_c.x -= 2;
        move.type = Castle_Queenside;
        moves_counter += reg_piece_handle_push_move(moves, move);
        move.to_c = orig;
        move.type = Normal;
    }
    if (can_castle_kingside && squares[home_side_value][5].piece == Empty && squares[home_side_value][6].piece == Empty && !is_square_under_attack(move.from_c.x, move.from_c.y, !current_turn) && !is_square_under_attack(5, home_side_value, !current_turn) && !is_square_under_attack(6, home_side_value, !current_turn)) {
        move.to_c.x += 2;
        move.type = Castle_Kingside;
        moves_counter += reg_piece_handle_push_move(moves, move);
        move.to_c = orig;
        move.type = Normal;
    }

    return moves_counter;
}

inline int Board::generate_knight_moves(std::vector<Move>& moves, int x, int y, bool ignore_turns) {
    int moves_counter = 0;

    if (!(is_correct_turn(x, y)) && !ignore_turns) {
        return;
    }

    Cords orig = Cords {x, y};
    Move move = {orig, orig, Normal};

    for (int i = -1; i < 2; i += 2) {
        for (int j = -1; j < 2; j += 2) {
            move.to_c.x += 1 * j;
            move.to_c.y += 2 * i;
            moves_counter += reg_piece_handle_push_move(moves, move);
            move.to_c = orig;

            move.to_c.x += 2 * j;
            move.to_c.y += 1 * i;
            moves_counter += reg_piece_handle_push_move(moves, move);
            move.to_c = orig;
        }
    }
    return moves_counter;
}

inline int Board::generate_slider_moves(Cords* increments, Move& move, std::vector<Move>& moves, int size, int x, int y) {
    int moves_counter = 0;
    for (int i = 0; i < size; i++) {
        move.to_c = sliding_pieces_incrementer(x, y, (increments + i)->x, (increments + i)->y);
        int temp = std::max(abs(move.to_c.x - x), abs(move.to_c.y - y));
        for (int j = 0; j < temp; j++) {
            moves_counter += reg_piece_handle_push_move(moves, move);
            move.to_c.x -= (increments + i)->x;
            move.to_c.y -= (increments + i)->y;
        }
    }
    return moves_counter;
}

inline int Board::generate_bishop_moves(std::vector<Move>& moves, int x, int y, bool ignore_turns) {
    int moves_counter = 0;

    if (!(is_correct_turn(x, y)) && !ignore_turns) {
        return 0;
    }

    Cords orig = Cords {x, y};
    Move move = {orig, orig, Normal};

    const int size = 4;
    Cords increments[size] = {Cords{-1, -1}, Cords{1, -1}, Cords{1, 1}, Cords{-1, 1}};

    moves_counter += generate_slider_moves(increments, move, moves, size, x, y);
    return moves_counter;
}

inline int Board::generate_rook_moves(std::vector<Move>& moves, int x, int y, bool ignore_turns) {
    int moves_counter = 0;

    if (!(is_correct_turn(x, y)) && !ignore_turns) {
        return 0;
    }

    Cords orig = Cords {x, y};
    Move move = {orig, orig, Normal};

    const int size = 4;
    Cords increments[size] = {Cords{0, -1}, Cords{-1, 0}, Cords{0, 1}, Cords{1, 0}};

    moves_counter += generate_slider_moves(increments, move, moves, size, x, y);
    return moves_counter;
}

inline int Board::generate_queen_moves(std::vector<Move>& moves, int x, int y, bool ignore_turns) {
    int moves_counter = 0;

    if (!(is_correct_turn(x, y)) && !ignore_turns) {
        return 0;
    }

    Cords orig = Cords {x, y};
    Move move = {orig, orig, Normal};

    const int size = 8;
    Cords increments[size] = {Cords{-1, -1}, Cords{1, -1}, Cords{1, 1}, Cords{-1, 1}, Cords{0, -1}, Cords{-1, 0}, Cords{0, 1}, Cords{1, 0}};

    moves_counter += generate_slider_moves(increments, move, moves, size, x, y);
    return moves_counter;
}

// TODO: Remove attacker_color maybe?
inline std::forward_list<Cords> Board::under_attack_cords(int x, int y, int attacker_color) {
    std::forward_list<Cords> attackers;

    // King check
    Cords enemy_king = current_turn == 1 ? white_king_loc : black_king_loc;
    int h1 = abs(enemy_king.x - x); int h2 = abs(enemy_king.y - y);
    if ((h1 <= 1 && h2 <= 1)  && !(h1 == 0 && h2 == 0)) {
        attackers.push_front(enemy_king);
    }


    // Pawn check
    int side_value;
    for (int i = -1; i < 2; i+=2) {
        side_value = current_turn == 1 ? 1 : -1;
        if (is_within_bounds(x+i, y+side_value)) {
            if (squares[y+side_value][x+i].piece == Pawn && squares[y+side_value][x+i].color != current_turn) {
                attackers.push_front(Cords{x+i, y+side_value});
            }
        }
    }


    // Knight check
    for (int i = -1; i < 2; i += 2) {
        for (int j = -1; j < 2; j += 2) {
            if (is_within_bounds(x + 1 * j, y + 2 * i)) {
                if (squares[y + 2 * i][x + 1 * j].piece == Knight && squares[y + 2 * i][x + 1 * j].color != current_turn) {
                    attackers.push_front(Cords{x + 1 * j, y + 2 * i});
                }
            }
            if (is_within_bounds(x + 2 * j, y + 1 * i)) {
                if (squares[y + 1 * i][x + 2 * j].piece == Knight && squares[y + 1 * i][x + 2 * j].color != current_turn) {
                    attackers.push_front(Cords{x + 2 * j, y + 1 * i});
                }
            }
        }
    }

    // Slider pieces check
    Cords increments[8] = {Cords{-1, -1}, Cords{1, -1}, Cords{1, 1}, Cords{-1, 1}, Cords{0, -1}, Cords{-1, 0}, Cords{0, 1}, Cords{1, 0}};
    Cords c;

    for (int i = 0; i < 8; i++) {
        c = sliding_pieces_incrementer(x, y, increments[i].x, increments[i].y, true);
        if (c.x == x && c.y == y) {
            continue;
        }
        if (squares[c.y][c.x].color != current_turn) {
            switch (squares[c.y][c.x].piece) {
                case Empty:
                case Pawn:
                case Knight:
                case King:
                    break;
                case Queen:
                    if (c.x == x || c.y == y || abs(y - c.y) == abs(x - c.x)) {
                        attackers.push_front(Cords{c.x, c.y});
                    }
                    break;
                case Bishop:
                    if (abs(y - c.y) == abs(x - c.x)) {
                        attackers.push_front(Cords{c.x, c.y});
                    }
                    break;
                case Rook:
                    if (c.x == x || c.y == y) {
                        attackers.push_front(Cords{c.x, c.y});
                    }
                    break;
                default:
                    std::cout << "Should not be reached under_attack_cords";
            }
        }
    }

    /*
    std::cout << "Searching: " << x << ' ' << y << std::endl;
    for (auto it = attackers.begin(); it != attackers.end(); ++it) {
        std::cout << it->x << ' ' << it->y << std::endl;
    }
     */


    return attackers;
}

inline bool Board::is_square_under_attack(int x, int y, int attacker_color) {
//        std::cout << x << ' ' << y << '|' << !under_attack_cords(x, y, attacker_color).empty() << '\n';
    return !under_attack_cords(x, y, attacker_color).empty();
}

inline bool Board::is_friendly_piece(int x, int y) {
    return (squares[y][x].piece != Empty && squares[y][x].color == current_turn);
}


bool Board::pawn_rules_subset(const Move &move, Move &validated_move) {
    int side_value, pawn_start_y;
    if (current_turn == 1) {
        side_value = 1;
        pawn_start_y = 1;
    }
    else {
        side_value = -1;
        pawn_start_y = 6;
    }

    if (move.to_c.y - move.from_c.y == side_value && move.to_c.x == move.from_c.x && squares[move.to_c.y][move.to_c.x].piece == Empty) {
        return true;
    }
    else if (abs(move.from_c.x - move.to_c.x) == 1 && move.to_c.y - move.from_c.y == side_value) {
        if (squares[move.to_c.y][move.to_c.x].piece != Empty) {
            return true;
        }
        else if (move.to_c.x == en_passant_cords.x && move.to_c.y == en_passant_cords.y) {
            validated_move.type = En_Passant;
            return true;
        }
    }
    else if (move.from_c.y - move.to_c.y == -2 * side_value && move.to_c.x - move.from_c.x == 0 && move.from_c.y == pawn_start_y && squares[move.to_c.y - side_value][move.to_c.x].piece == Empty) {
        return true;
    }
    return false;
}

bool Board::is_pawn_move_valid(Move move, Move& validated_move) {
    int opposite_side_value;
    if (current_turn == 1) {
        opposite_side_value = 7;
    }
    else {
        opposite_side_value = 0;
    }
    if (move.type == Normal && move.to_c.y == opposite_side_value) {
        return false;
    }
    return pawn_rules_subset(move, validated_move);
}

bool Board::is_king_move_valid(Move move, Move& validated_move) {
    int home_side_value = current_turn == 1 ? 0 : 7;
    int can_castle_queenside = current_turn ? black_can_castle_queenside : white_can_castle_queenside;
    int can_castle_kingside = current_turn ? black_can_castle_kingside : white_can_castle_kingside;

    if (is_square_under_attack(move.to_c.x, move.to_c.y, !current_turn)) {
        return false;
    }
    else if (abs(move.from_c.y - move.to_c.y) <= 1 && abs(move.from_c.x - move.to_c.x) <= 1) {
        return true;
    }
    else if (move.to_c.x == 2 && can_castle_queenside && !is_square_under_attack(3, home_side_value, !current_turn) && squares[home_side_value][3].piece == Empty && squares[home_side_value][2].piece == Empty && squares[home_side_value][1].piece == Empty && !is_square_under_attack(move.from_c.x, move.from_c.y, !current_turn)) {
        validated_move.type = Castle_Queenside;
        return true;
    }
    else if (move.to_c.x == 6 && can_castle_kingside && !is_square_under_attack(5, home_side_value, !current_turn) && squares[home_side_value][5].piece == Empty && squares[home_side_value][6].piece == Empty && !is_square_under_attack(move.from_c.x, move.from_c.y, !current_turn)) {
        validated_move.type = Castle_Kingside;
        return true;
    }

    return false;
}

bool Board::is_knight_move_valid(int from_x, int from_y, int to_x, int to_y) {
    return ((abs(from_x - to_x) == 1 && (abs(from_y - to_y) == 2)) || (abs(from_x - to_x) == 2 && (abs(from_y - to_y) == 1)));
}


bool Board::sliding_path_check(int from_x, int from_y, int to_x, int to_y) {
    Cords c = sliding_pieces_incrementer(from_x, from_y, sgn(to_x-from_x), sgn(to_y-from_y));
    return (abs(from_x - c.x) >= abs(from_x - to_x) && abs(from_y - c.y) >= abs(from_y - to_y));
}

bool Board::is_bishop_move_valid(int from_x, int from_y, int to_x, int to_y) {
    if (abs(from_x - to_x) != abs(from_y - to_y)) {
        return false;
    }
    else {
        return sliding_path_check(from_x, from_y, to_x, to_y);
    }
}

bool Board::is_rook_move_valid(int from_x, int from_y, int to_x, int to_y) {
    if (!(from_x - to_x == 0 || from_y - to_y == 0)) {
        return false;
    }
    else {
        return sliding_path_check(from_x, from_y, to_x, to_y);
    }
}

bool Board::is_queen_move_valid(int from_x, int from_y, int to_x, int to_y) {
    if (abs(from_x - to_x) != abs(from_y - to_y) && !(from_x - to_x == 0 || from_y - to_y == 0)) {
        return false;
    }
    else {
        return sliding_path_check(from_x, from_y, to_x, to_y);
    }
}


inline bool Board::is_correct_turn(int x, int y) {
    return (squares[y][x].color == current_turn);
}

inline bool Board::is_within_bounds(int x, int y) {
    return (0 <= x && x <= 7 && 0 <= y && y <= 7);
}

inline bool Board::follows_pin_rules(Move move) {
    Cords king_c = current_turn == 1 ? black_king_loc : white_king_loc;

    // For some reason, pinned_by.find does work as expected. Cords initialization issues
    Cords& c = pinned_by[move.from_c];
//        std::cout << c.x << ' ' << c.y;
    if (c == Cords{-1, -1}) {
        pinned_by.erase(move.from_c);
        return true;
    }
    else {
        if (move.to_c == c) {
            return true;
        }
        else if (!is_in_between(king_c, c, move.to_c)) {
            return false;
        }
        switch(squares[c.y][c.x].piece) {
            case Queen:
                if (c.x == move.to_c.x && king_c.x == move.to_c.x) {
                    return true;
                }
                else if (c.y == move.to_c.y && king_c.y == move.to_c.y) {
                    return true;
                }
                else if (abs(move.to_c.y - c.y) == abs(move.to_c.x - c.x)) {
                    return true;
                }
            case Bishop:
                if (abs(move.to_c.y - c.y) == abs(move.to_c.x - c.x)) {
                    return true;
                }
                break;
            case Rook:
                if (c.x == move.to_c.x || c.y == move.to_c.y) {
                    return true;
                }
                break;
            default:
                std::cout << "should not have been reached follows_pin_rules";
        }
    }
    return false;
}

inline bool Board::is_following_piece_rules(Move move, Move& validated_move) {

    switch (squares[move.from_c.y][move.from_c.x].piece) {

        case Pawn:
            return is_pawn_move_valid(move, validated_move);
            break;
        case King:
            return is_king_move_valid(move, validated_move);
            break;
        case Knight:
            return is_knight_move_valid(move.from_c.x, move.from_c.y, move.to_c.x, move.to_c.y);
            break;
        case Bishop:
            return is_bishop_move_valid(move.from_c.x, move.from_c.y, move.to_c.x, move.to_c.y);
            break;
        case Rook:
            return is_rook_move_valid(move.from_c.x, move.from_c.y, move.to_c.x, move.to_c.y);
            break;
        case Queen:
            return is_queen_move_valid(move.from_c.x, move.from_c.y, move.to_c.x, move.to_c.y);
            break;
        default:
            std::cout << "Something really bad must have happened to get here" << std::endl;
            break;
    }

    return false;
}

Move Board::is_move_valid(Move move) {
    Move validated_move = move;


    switch (validated_move.type) {
        case Promote_to_Queen:
        case Promote_to_Rook:
        case Promote_to_Bishop:
        case Promote_to_Knight:
            break;
        default:
            validated_move.type = Normal;
    }

    if (!does_pass_basic_piece_checks(move)) {
        validated_move.type = Illegal;
    }
    else if (!is_following_piece_rules(move, validated_move)) {
        validated_move.type = Illegal;
    }

//    std::cout << x << " " << y << std::endl;
    return validated_move;
}

bool Board::is_trying_to_promote(Move move) {
    int side_value = current_turn == 1 ? 7 : 0;

    // This is stupid but whatev
    Move holder_move = move;

    if (squares[move.from_c.y][move.from_c.x].piece != Pawn || !pawn_rules_subset(move, holder_move) || move.to_c.y != side_value) {
        return false;
    }
    else if (!does_pass_basic_piece_checks(move)) {
        return false;
    }
    return true;
}


inline void Board::add_to_enemy_piece_values(int i) {
    if (current_turn) {
        white_piece_values += i;
    }
    else {
        black_piece_values += i;
    }
}

inline void Board::add_to_home_piece_values(int i) {
    if (current_turn) {
        black_piece_values += i;
    }
    else {
        white_piece_values += i;
    }
}

// Legal moves must be generated before game end evals are called
inline bool Board::has_been_checkmated(const std::vector<Move>& moves) {
    /*
    std::cout << '\n' << '\n' << '\n' << '\n';
    for (auto it = legal_moves.begin(); it != legal_moves.end(); ++it) {
        print_move(*it);
    }
     */
    Cords king_c = current_turn == 1 ? black_king_loc : white_king_loc;
    return moves.empty() && is_square_under_attack(king_c.x, king_c.y, !current_turn);
}

inline bool Board::is_draw(const std::vector<Move>& moves) {
    Cords king_c = current_turn == 1 ? black_king_loc : white_king_loc;
    if (moves.empty() && !is_square_under_attack(king_c.x, king_c.y, !current_turn)) {
        return true;
    }
    /*
    else if (previous_board_positions[remove_FEN_counters(generate_FEN())] == 2) {
        return true;
    }
     */
    else {
        return false;
    }
}

void Board::process_board_changes(const Move &move) {
    squares[move.to_c.y][move.to_c.x] = squares[move.from_c.y][move.from_c.x];
    Square square;
    square.piece = Empty;
    square.color = 0;
    squares[move.from_c.y][move.from_c.x] = square;

    // set some values for side-specific move patterns
    int castle_side_value, en_passant_side_value;
    if (current_turn == 1) {
        castle_side_value = 0;
        en_passant_side_value = 4;
    }
    else {
        castle_side_value = 7;
        en_passant_side_value = 3;
    }


    // Handle promotions, castling, en_passant
    switch (move.type) {
        case Promote_to_Queen:
            squares[move.to_c.y][move.to_c.x].piece = Queen;
            add_to_home_piece_values(QUEEN_VALUE - PAWN_VALUE);
            break;
        case Promote_to_Rook:
            squares[move.to_c.y][move.to_c.x].piece = Rook;
            add_to_home_piece_values(ROOK_VALUE - PAWN_VALUE);
            break;
        case Promote_to_Bishop:
            squares[move.to_c.y][move.to_c.x].piece = Bishop;
            add_to_home_piece_values(BISHOP_VALUE - PAWN_VALUE);
            break;
        case Promote_to_Knight:
            squares[move.to_c.y][move.to_c.x].piece = Knight;
            add_to_home_piece_values(KNIGHT_VALUE - PAWN_VALUE);
            break;
        case Castle_Queenside:
            squares[castle_side_value][3] = squares[castle_side_value][0];
            squares[castle_side_value][0].piece = Empty;
            squares[castle_side_value][0].color = 0;
            break;
        case Castle_Kingside:
            squares[castle_side_value][5] = squares[castle_side_value][7];
            squares[castle_side_value][7].piece = Empty;
            squares[castle_side_value][7].color = 0;
            break;
        case En_Passant:
            squares[en_passant_side_value][move.to_c.x].piece = Empty;
            add_to_enemy_piece_values(-PAWN_VALUE);
            break;
        case Normal:
            break;
        case Illegal:
        default:
            std::cout << "This should not have been reached (process_board_changes).";
            break;
    }
}

std::string Board::remove_FEN_counters(std::string in_str) {
    std::string str;

    bool flag = false;
    int pos;

    for (pos = in_str.length() - 1; pos >= 0; pos--) {
        if (in_str[pos] == ' ') {
            if (!flag) {
                flag = true;
            }
            else {
                break;
            }
        }
//            std::cout << pos << std::endl;
    }
    str = in_str.substr(0, pos);
//        std::cout << str;
    return str;
}

void Board::clear_attacks_on_king() {
    attacks_on_the_king.clear();
}

void Board::process_move(Move move) {

    // Save move info onto past moves stack
    Move_data move_data;
    move_data.move = move;
    move_data.captured_piece = squares[move.to_c.y][move.to_c.x].piece;
    move_data.previous_en_passant_cords = en_passant_cords;
    move_data.white_can_castle_kingside = white_can_castle_kingside;
    move_data.white_can_castle_queenside = white_can_castle_queenside;
    move_data.black_can_castle_kingside = black_can_castle_kingside;
    move_data.black_can_castle_queenside = black_can_castle_queenside;

    move_stack.push_front(move_data);



    // If square was captured, change the enemy piece values accordingly
    add_to_enemy_piece_values(-piece_to_value(squares[move.to_c.y][move.to_c.x].piece));


    // Add the current board position for 3-move repition check

//        previous_board_positions[remove_FEN_counters(generate_FEN())] += 1;


    // Check to see if castling is invalidated

    // see if rook was captured/moved
    int eval_x = move.to_c.x;
    int eval_y = move.to_c.y;
    for (int i = 0; i < 2; i++) {
        if (eval_x == 0 && eval_y == 0) {
            black_can_castle_queenside = false;
        }
        else if (eval_x == 7 && eval_y == 0) {
            black_can_castle_kingside = false;
        }
        else if (eval_x == 0 && eval_y == 7) {
            white_can_castle_queenside = false;
        }
        else if (eval_x == 7 && eval_y == 7) {
            white_can_castle_kingside = false;
        }

        eval_x = move.from_c.x;
        eval_y = move.from_c.y;
    }

    // Check if king moves
    if (squares[move.from_c.y][move.from_c.x].piece == King) {
        if (current_turn == 1) {
            black_can_castle_kingside = false;
            black_can_castle_queenside = false;
        }
        else {
            white_can_castle_kingside = false;
            white_can_castle_queenside = false;
        }
    }




    // TODO: use ternary ops instead of if-else

    // Check if En Passant cords need to be set
    if (squares[move.from_c.y][move.from_c.x].piece == Pawn && abs(move.from_c.y - move.to_c.y) == 2) {
        if (current_turn == 1) {
            en_passant_cords.x = move.to_c.x;
            en_passant_cords.y = move.to_c.y - 1;
        }
        else {
            en_passant_cords.x = move.to_c.x;
            en_passant_cords.y = move.to_c.y + 1;
        }
    }
    else {
        // Clear En Passant
        en_passant_cords.x = -1;
        en_passant_cords.y = -1;
    }

    // If moved to/from squares in pin paths, delete old pins, prep recalculate pins
    // The reason this is done pre-actual move is because delete_pins *must* be called before the board is changed.
    Cords king_c;
    int generate_pins_info[2] = {0, 0};
    int increments_white[8] = {0};
    int increments_black[8] = {0};
    int* increments;
    king_c = current_turn == 1 ? black_king_loc : white_king_loc;

    if (!(move.type == Castle_Kingside || move.type == Castle_Queenside || move.type == En_Passant || move.from_c == king_c)) {
        for (int i = 0; i < 2; i++) {

            king_c = i == 1 ? black_king_loc : white_king_loc;
            increments = i == 1 ? increments_black : increments_white;


            if (abs(move.from_c.x - king_c.x) == abs(move.from_c.y - king_c.y) || move.from_c.x == king_c.x || move.from_c.y == king_c.y) {
                *(increments + increment_to_index(Cords{sgn(move.from_c.x - king_c.x), sgn(move.from_c.y - king_c.y)})) = 1;
                generate_pins_info[i] = 1;
            }

            if (abs(move.to_c.x - king_c.x) == abs(move.to_c.y - king_c.y) || move.to_c.x == king_c.x || move.to_c.y == king_c.y) {
                *(increments + increment_to_index(Cords{sgn(move.to_c.x - king_c.x), sgn(move.to_c.y - king_c.y)})) = 1;
                generate_pins_info[i] = 1;
            }

            if (generate_pins_info[i]) {
                delete_pins(i, increments);
            }
        }
    } else {
        delete_pins(0, incre8);
        delete_pins(1, incre8);
        generate_pins_info[0] = 2;
        generate_pins_info[1] = 2;
    }

    if (squares[move.from_c.y][move.from_c.x].piece == King) {
        if (current_turn == 1) {
            black_king_loc = move.to_c;
        }
        else {
            white_king_loc = move.to_c;
        }
    }

    // Actual act of moving pieces
    process_board_changes(move);



    // Finish pins
    for (int i = 0; i < 2; i++) {
        increments = i == 1 ? increments_black : increments_white;
        if (generate_pins_info[i] == 1) {

            /*
            for (int x = 0; x < 8; x++) {
                std::cout << *(increments + x) << ' ';
            }
            std::cout << '\n';
             */
            generate_pins(i, increments);
        }
        else if (generate_pins_info[i] == 2) {
            generate_pins(i, incre8);
        }
    }

    /*
    debug_attacked_squares(current_turn);
    auto copy1 = attacking;
    auto copy2 = attacked_by_black;
    auto copy3 = attacked_by_white;
    generate_attacked_squares();
    debug_attacked_squares(current_turn);

    attacking = copy1;
    attacked_by_black = copy2;
    attacked_by_white = copy3;
    */

    current_turn = !current_turn;

    /*
    generate_moves(legal_moves);

    if (has_been_checkmated()) {
        std::cout << "Checkmate!" << std::endl;
    }
    if (is_draw()) {
        std::cout << "Draw has been reached. " << std::endl;
    }
     */

//        debug_print();
//        undo_last_move();
//        debug_print();
//        std::cout << generate_FEN();
}

void Board::undo_last_move() {
    Move_data move_data = *(move_stack.begin());
//        previous_board_positions[remove_FEN_counters(generate_FEN())] -= 1;

    current_turn = !current_turn;

    Cords king_c;
    int generate_pins_info[2] = {0, 0};
    int increments_white[8] = {0};
    int increments_black[8] = {0};
    int* increments;
    king_c = current_turn == 1 ? black_king_loc : white_king_loc;

    if (!(move_data.move.type == Castle_Kingside || move_data.move.type == Castle_Queenside || move_data.move.type == En_Passant || move_data.move.to_c == king_c)) {
        for (int i = 0; i < 2; i++) {

            king_c = i == 1 ? black_king_loc : white_king_loc;
            increments = i == 1 ? increments_black : increments_white;

            if (abs(move_data.move.from_c.x - king_c.x) == abs(move_data.move.from_c.y - king_c.y) || move_data.move.from_c.x == king_c.x || move_data.move.from_c.y == king_c.y) {
                *(increments + increment_to_index(Cords{sgn(move_data.move.from_c.x - king_c.x), sgn(move_data.move.from_c.y - king_c.y)})) = 1;
                generate_pins_info[i] = 1;
            }

            if (abs(move_data.move.to_c.x - king_c.x) == abs(move_data.move.to_c.y - king_c.y) || move_data.move.to_c.x == king_c.x || move_data.move.to_c.y == king_c.y) {
                *(increments + increment_to_index(Cords{sgn(move_data.move.to_c.x - king_c.x), sgn(move_data.move.to_c.y - king_c.y)})) = 1;
                generate_pins_info[i] = 1;
            }

            if (generate_pins_info[i]) {
                delete_pins(i, increments);
            }
        }
    } else {
        delete_pins(0, incre8);
        delete_pins(1, incre8);
        generate_pins_info[0] = 2;
        generate_pins_info[1] = 2;
    }



    if (squares[move_data.move.to_c.y][move_data.move.to_c.x].piece == King) {
        if (current_turn == 1) {
        // Change the king_loc as well
            black_king_loc = move_data.move.from_c;
        }
        else {
            white_king_loc = move_data.move.from_c;
        }
    }



    squares[move_data.move.from_c.y][move_data.move.from_c.x] = squares[move_data.move.to_c.y][move_data.move.to_c.x];
    squares[move_data.move.to_c.y][move_data.move.to_c.x].piece = move_data.captured_piece;
    squares[move_data.move.to_c.y][move_data.move.to_c.x].color = !current_turn;

    // Take the captured piece, and add its value back to enemy piece_values
    add_to_enemy_piece_values(piece_to_value(move_data.captured_piece));

    // set some values for side-specific move patterns
    int castle_side_value, en_passant_side_value;
    if (current_turn == 1) {
        castle_side_value = 0;
        en_passant_side_value = 4;
    }
    else {
        castle_side_value = 7;
        en_passant_side_value = 3;
    }


    // Handle promotions, castling, en_passant
    switch (move_data.move.type) {
        case Promote_to_Queen:
            // return the piece_values back to what they were before promotion
            add_to_home_piece_values(PAWN_VALUE - QUEEN_VALUE);
            squares[move_data.move.from_c.y][move_data.move.from_c.x].piece = Pawn;
            break;
        case Promote_to_Rook:
            add_to_home_piece_values(PAWN_VALUE - ROOK_VALUE);
            squares[move_data.move.from_c.y][move_data.move.from_c.x].piece = Pawn;
            break;
        case Promote_to_Bishop:
            add_to_home_piece_values(PAWN_VALUE - BISHOP_VALUE);
            squares[move_data.move.from_c.y][move_data.move.from_c.x].piece = Pawn;
            break;
        case Promote_to_Knight:
            add_to_home_piece_values(PAWN_VALUE - KNIGHT_VALUE);
            squares[move_data.move.from_c.y][move_data.move.from_c.x].piece = Pawn;
            break;
        case Castle_Queenside:
            squares[castle_side_value][0] = squares[castle_side_value][3];
            squares[castle_side_value][3].piece = Empty;
            squares[castle_side_value][3].color = 0;
            break;
        case Castle_Kingside:
            squares[castle_side_value][7] = squares[castle_side_value][5];
            squares[castle_side_value][5].piece = Empty;
            squares[castle_side_value][5].color = 0;
            break;
        case En_Passant:
            squares[en_passant_side_value][move_data.move.to_c.x].piece = Pawn;
            squares[en_passant_side_value][move_data.move.to_c.x].color = !current_turn;
            add_to_enemy_piece_values(PAWN_VALUE);
            break;
        case Normal:
            break;
        case Illegal:
        default:
            std::cout << "This should not have been reached (process_move).";
            break;
    }


    for (int i = 0; i < 2; i++) {
        increments = i == 1 ? increments_black : increments_white;
        if (generate_pins_info[i] == 1) {
            generate_pins(i, increments);
        }
        else if (generate_pins_info[i] == 2) {
            generate_pins(i, incre8);
        }
    }

    en_passant_cords = move_data.previous_en_passant_cords;
    white_can_castle_kingside = move_data.white_can_castle_kingside;
    white_can_castle_queenside = move_data.white_can_castle_queenside;
    black_can_castle_kingside = move_data.black_can_castle_kingside;
    black_can_castle_queenside = move_data.black_can_castle_queenside;

    move_stack.pop_front();
}

void Board::debug_piece_values() {
    std::cout << "white_piece_values: " << white_piece_values << " | black_piece_values: " << black_piece_values << '\n';
}

long Board::Perft(int depth /* assuming >= 1 */) {
    long nodes = 0;
    int n_moves = 0;

    std::vector<Move> moves;
    moves.reserve(256);
    n_moves = generate_moves(moves);

    if (depth == 0) {
        return 1;
    }

    /*
    if (depth == 1) {
        return n_moves;
    }
     */

    for (auto it = moves.begin(); it != moves.end(); ++it) {
        process_move(*it);
        nodes += Perft(depth - 1);
        undo_last_move();
    }
    return nodes;
}

void Board::sort_moves(std::vector<Move>& moves) {
    int score;

    // Score all the moves
    for (auto it = moves.begin(); it != moves.end(); ++it) {
        score = 0;
        if (squares[it->to_c.y][it->to_c.x].piece != Empty) {
            score += 70;
            score += piece_to_value(squares[it->to_c.y][it->to_c.x].piece) - piece_to_value(squares[it->from_c.y][it->from_c.x].piece);
        }
        if (it->type == Promote_to_Queen || it->type == Promote_to_Rook || it->type == Promote_to_Bishop || it->type == Promote_to_Knight) {
            score += 100;
        }
        else if (it->type == Castle_Kingside || it->type == Castle_Queenside) {
            score += 60;
        }
        it->score = score;
    }

    sort(moves.begin(), moves.end(), move_cmp);
}


inline int Board::static_eval(/*std::forward_list<Move>& moves*/) {
    int eval = 0;

    eval += white_piece_values - black_piece_values;


    eval *= current_turn ? -1 : 1;
    return eval;
}

int Board::negamax(int depth, int alpha, int beta) {
//        auto t1 = std::chrono::high_resolution_clock::now();
    std::vector<Move> moves;
    moves.reserve(256);
    generate_moves(moves);
    sort_moves(moves);
//        auto t2 = std::chrono::high_resolution_clock::now();
//        std::chrono::duration<double, std::milli> ms_double = t2 - t1;
//        s_timer += ms_double.count();



    if (has_been_checkmated(moves)) {
        return get_current_turn() == 0 ? 2000000 : -2000000;
    }
    else if (is_draw(moves)) {
        return 0;
    }
    else if (depth == 0) {
//            return quiescence_search(alpha, beta);
        return static_eval();
    }


    for (auto it = moves.begin(); it != moves.end(); ++it) {
        process_move(*it);
        int eval = -negamax(depth - 1, -beta, -alpha);
        undo_last_move();
        if (eval >= beta) {
            return beta;
        }
        alpha = std::max(alpha, eval);
    }

    return alpha;
}


int Board::quiescence_search(int alpha, int beta) {
    int stand_pat = static_eval();
    if (stand_pat >= beta) {
        return beta;
    }
    if (alpha < stand_pat) {
        alpha = stand_pat;
    }


    std::vector<Move> moves;
    moves.reserve(256);
    generate_moves(moves);
    sort_moves(moves);

    if (has_been_checkmated(moves)) {
        return get_current_turn() == 0 ? 2000000 : -2000000;
    }
    else if (is_draw(moves)) {
        return 0;
    }

    for (auto it = moves.begin(); it != moves.end(); ++it)  {
        if (squares[it->to_c.y][it->to_c.x].piece != Empty) {
            process_move(*it);
            int score = -quiescence_search(-beta, -alpha);
            undo_last_move();

            if (score >= beta) {
                return beta;
            }
            alpha = std::max(alpha, score);
        }
    }
    return alpha;
}


Move Board::request_move(Move move){
    // This functions takes in a move requested by the board, and returns the correct type of it is,
    // whilst updating the internal board appropriately. It checks if the move is illegal, and whether it is a special move
    Move validated_move = is_move_valid(move);
    if (validated_move.type != Illegal) {
        process_move(validated_move);
    }
    return validated_move;
}

