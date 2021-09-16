//
// Created by Andrew Xia on 9/15/21.
//

#include "tests.hpp"

void perft_tests() {
    Board board("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
    OpeningBook ob;
    TT tt;
    std::atomic<bool> b;
    TimeHandler th(b);


    Search s(board, tt, ob, th);
    std::cout << s.perft(5);

    std::cout << std::endl;
}
