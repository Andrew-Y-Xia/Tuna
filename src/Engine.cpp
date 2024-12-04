//
//  Engine.cpp
//  Bitboard Chess
//
//  Created by Andy on 8/15/21.
//  Copyright © 2021 Andy. All rights reserved.
//

#include "Engine.hpp"


Engine::Engine(Thread::SafeQueue<std::vector<std::string>>& c, std::atomic<bool>& b) : cmd_queue(c),
                                                                                       should_end_search(b) {};

void Engine::loop() {
    Board board;
    TT tt;
    OpeningBook opening_book;
    TimeHandler inf_time(should_end_search);

    while (true) {
        std::vector<std::string> cmd = cmd_queue.dequeue();

        try {
            if (cmd.at(0) == "quit") {
                return;
            } else if (cmd.at(0) == "go") {
                if (cmd.at(1) == "perft") {
                    int perft_depth = std::stoi(cmd.at(2));

                    MoveList moves;
                    board.generate_moves(moves);

                    long perft_sum = 0;

                    auto t1 = std::chrono::high_resolution_clock::now();

                    for (int i = 0; i < moves.size(); i++) {

                        board.make_move(moves[i]);

                        Search search(board, tt, opening_book, inf_time);
                        long perft_score = search.perft(perft_depth - 1);
                        perft_sum += perft_score;
                        std::ostringstream buffer;
                        buffer << move_to_str(moves[i], true) << ": ";
                        buffer << perft_score << '\n';
                        get_synced_cout().print(buffer.str());

                        board.unmake_move();

                    }
                    auto t2 = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double, std::milli> ms_double = t2 - t1;
                    std::ostringstream buffer;
                    buffer << "\nNodes Searched: " << perft_sum << '\n';
                    buffer << "Time: " << ms_double.count() << "ms\n\n";
                    get_synced_cout().print(buffer.str());

                } else if (cmd.at(1) == "infinite") {
                    Search search(board, tt, opening_book, inf_time);
                    search.find_best_move(64);
                } else {
                    int max_depth = 64;
                    double time_ms = 0;
                    TimerType t_type = constant_time;
                    int wtime = 0;
                    int btime = 0;
                    int winc  = 0;
                    int binc  = 0;
                    int moves_to_go = -1;

                    for (int i = 1; i < cmd.size(); i += 2) {
                        if (cmd.at(i) == "movetime") {
                            time_ms = std::stoi(cmd.at(i + 1));
                            time_ms -= 100;
                        } else if (cmd.at(i) == "wtime") {
                            wtime = std::stoi(cmd.at(i + 1));
                        } else if (cmd.at(i) == "btime") {
                            btime = std::stoi(cmd.at(i + 1));
                        } else if (cmd.at(i) == "movestogo") {
                            moves_to_go = std::stoi(cmd.at(i + 1));
                        } else if (cmd.at(i) == "winc") {
                            winc = std::stoi(cmd.at(i + 1));
                        } else if (cmd.at(i) == "binc") {
                            binc = std::stoi(cmd.at(i + 1));
                        }
                    }

                    if (moves_to_go == -1) {
                        moves_to_go = 40;
                    }

                    int current_turn_time = board.get_current_turn() == WHITE ? wtime : btime;
                    int current_inc = board.get_current_turn() == WHITE ? winc : binc;
                    if (current_turn_time && moves_to_go) {
                        double calc_time = ((double) current_turn_time) / (moves_to_go + 2);
                        if (time_ms != 0) {
                            calc_time = std::min((double) time_ms, calc_time);
                        }
                        time_ms = calc_time;
                    }
                    time_ms += current_inc * 0.5;

                    TimeHandler time_handler(should_end_search, t_type, time_ms);
                    Search search(board, tt, opening_book, time_handler);
                    search.find_best_move(max_depth);
                }
            } else if (cmd.at(0) == "position") {
                int j = 1;
                while (j < cmd.size()) {
                    if (cmd.at(j) == "fen") {
                        std::string fen;
                        int j_save = j;
                        for (int i = j + 1; i < j_save + 7; i++) {
                            fen += cmd.at(i) + ' ';
                        }
                        fen.pop_back();
                        board = Board(fen);
                    } else if (cmd.at(j) == "startpos") {
                        board = Board();
                    } else if (cmd.at(j) == "moves") {
                        for (int i = j + 1; i < cmd.size(); i++) {
                            Move move = board.read_LAN(cmd.at(i));
                            board.make_move(move);
                        }
                    }
                    j++;
                }
            } else if (cmd.at(0) == "printboard") {
                board.print_board();
            } else if (cmd.at(0) == "ucinewgame") {
                board = Board();
                tt.clear();
                opening_book.reset();
            }
        }
        catch (std::out_of_range& e) {
            std::cerr << "Insufficient parameters\n";
        }
    }
}

std::thread Engine::spawn() {
    return std::thread(&Engine::loop, this);
}

