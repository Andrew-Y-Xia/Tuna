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

void test_see(std::string s, std::string move, int value) {
    Board b(s);
    int result = b.static_exchange_eval(b.read_LAN(move));
//    std::cout << result << std::endl;
    assert(result == value);
}

void tests() {
    test_see("1k1r4/1pp4p/p7/4p3/8/P5P1/1PP4P/2K1R3 w - - 0 1", "e1e5", 100);
    test_see("4R3/2r3p1/5bk1/1p1r3p/p2PR1P1/P1BK1P2/1P6/8 b - - 0 1", "h5g4", 0);
    test_see("4R3/2r3p1/5bk1/1p1r1p1p/p2PR1P1/P1BK1P2/1P6/8 b - - 0 1", "h5g4", 0);
    test_see("4r1k1/5pp1/nbp4p/1p2p2q/1P2P1b1/1BP2N1P/1B2QPPK/3R4 b - - 0 1", "g4f3", 0);
    test_see("2r1r1k1/pp1bppbp/3p1np1/q3P3/2P2P2/1P2B3/P1N1B1PP/2RQ1RK1 b - - 0 1", "d6e5", PAWN_VALUE);



    std::cout << std::endl;
}
