//
//  Opening_book.cpp
//  Bitboard Chess
//
//  Created by Andrew Xia on 7/21/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Opening_book.hpp"

std::vector<std::vector<Move>> opening_lines;

std::vector<std::string> split(const std::string& line);

OpeningBook::OpeningBook() {
    use_book = true;
}

bool OpeningBook::can_use_book() {
    return use_book;
}

void OpeningBook::set_use_book(bool b) {
    use_book = b;
}

Move OpeningBook::request(std::vector<move_data> move_stack) {
    Move illegal_move;
    illegal_move.set_as_illegal();

    std::vector<int> possible_line_indices;

    std::mt19937 generator(
            static_cast<unsigned int>(std::chrono::high_resolution_clock::now().time_since_epoch().count()));


    if (move_stack.size() == 0) {
        std::uniform_int_distribution<int> full_distribution(0, (int) opening_lines.size() - 1);
        return opening_lines[full_distribution(generator)][0];
    }

    if (!use_book) {
        return illegal_move;
    }

    int line_index = 0;
    for (auto line = opening_lines.begin(); line != opening_lines.end(); line++, line_index++) {
        if (move_stack.size() >= line->size()) {
            continue;
        }
        bool match_flag = true;
        for (int move_index = 0; move_index < move_stack.size(); move_index++) {
            // Compare moves
            if ((*line)[move_index] != move_stack[move_index].move) {
                match_flag = false;
                break;
            }
        }
        if (match_flag) {
            possible_line_indices.push_back(line_index);
        }
    }

    // No lines were found
    if (possible_line_indices.empty()) {
        use_book = false;
        return illegal_move;
    }



    // Distribution on which to apply the generator (uniform, from 0 to 2^64 - 1)
    std::uniform_int_distribution<int> distribution(0, (int) possible_line_indices.size() - 1);
    int random_int = distribution(generator);

    return opening_lines[possible_line_indices[random_int]][move_stack.size()];
}

void OpeningBook::reset() {
    use_book = true;
}


void init_opening_book() {
    // Loads opening book into memory
    std::string line;
    std::ifstream file(resource_path() + "opening_book.txt");
    if (file.is_open()) {
        // 5000 is the number of lines in opening_book.txt
        // Every line is a game (PGN format)
        opening_lines.reserve(5000);

        while (std::getline(file, line)) {
            std::vector<std::string> move_strs = split(line);



            // str.split() end

            move_strs.pop_back();

            Board board;
            std::vector<Move> move_list;

            bool skip_flag = false;

            for (auto it = move_strs.begin(); it != move_strs.end(); it++) {
                if (it->front() == '{') {
                    skip_flag = true;
                    continue;
                }
                if (it->front() == '}') {
                    skip_flag = false;
                    continue;
                }
                if (it->back() == '.' || skip_flag) {
                    continue;
                }

                Move move = board.read_SAN(*it);
                move_list.push_back(move);
                board.make_move(move);
            }


            opening_lines.push_back(move_list);
        }
        file.close();
    } else {
        std::cout << "Opening book failed to init";
    }
}
