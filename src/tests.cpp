//
// Created by Andrew Xia on 9/15/21.
//

#include "tests.hpp"

void test_perft(std::string fen, unsigned int depth, long target) {
    Board board(fen);
    OpeningBook ob;
    TT tt;
    std::atomic<bool> b;
    TimeHandler th(b);


    Search s(board, tt, ob, th);
    long result = s.perft(depth);

    if (target != result) {
        std::cout << "Perft test failed: " << fen << " depth: " << depth << " Expected value: " << target << " Result: "
                  << result << '\n';
    }
}

void test_see(std::string s, std::string move, int value) {
    Board b(s);
    int result = b.static_exchange_eval(b.read_LAN(move));
    if (result != value) {
        std::cout << "SEE test failed: " << s << " move: " << move << " Expected value: " << value << " Result: "
                  << result << '\n';
    }
}

// Perft tests take a while
void perft_summary_tests() {
    test_perft("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", 6, 119060324);
    test_perft("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", 5, 193690690);
    test_perft("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", 7, 178633661);
    test_perft("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 5, 15833292);
    test_perft("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", 5, 89941194);
    test_perft("8/8/b2p3p/7k/7P/K5P1/4p3/3B4 b - - 1 71", 7, 31728295);
}

void tests() {
    test_see("1k1r4/1pp4p/p7/4p3/8/P5P1/1PP4P/2K1R3 w - - 0 1", "e1e5", 100);
    test_see("4R3/2r3p1/5bk1/1p1r3p/p2PR1P1/P1BK1P2/1P6/8 b - - 0 1", "h5g4", 0);
    test_see("4R3/2r3p1/5bk1/1p1r1p1p/p2PR1P1/P1BK1P2/1P6/8 b - - 0 1", "h5g4", 0);
    test_see("4r1k1/5pp1/nbp4p/1p2p2q/1P2P1b1/1BP2N1P/1B2QPPK/3R4 b - - 0 1", "g4f3", 0);
    test_see("2r1r1k1/pp1bppbp/3p1np1/q3P3/2P2P2/1P2B3/P1N1B1PP/2RQ1RK1 b - - 0 1", "d6e5", PAWN_VALUE);
    test_see("7r/5qpk/p1Qp1b1p/3r3n/BB3p2/5p2/P1P2P2/4RK1R w - - 0 1", "e1e8", 0);
    test_see("6rr/6pk/p1Qp1b1p/2n5/1B3p2/5p2/P1P2P2/4RK1R w - - 0 1", "e1e8", -ROOK_VALUE);
    test_see("7r/5qpk/2Qp1b1p/1N1r3n/BB3p2/5p2/P1P2P2/4RK1R w - - 0 1", "e1e8", -ROOK_VALUE);
//    test_see("6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - - 0 1", "f7f8q", BISHOP_VALUE-PAWN_VALUE);
//    test_see("6RR/4bP2/8/8/5r2/3K4/5p2/4k3 w - - 0 1", "f7f8n", KNIGHT_VALUE-PAWN_VALUE);
//    test_see("7R/4bP2/8/8/1q6/3K4/5p2/4k3 w - - 0 1", "f7f8r", -PAWN_VALUE);
    test_see("8/4kp2/2npp3/1Nn5/1p2PQP1/7q/1PP1B3/4KR1r b - - 0 1", "h1f1", 0);
    test_see("8/4kp2/2npp3/1Nn5/1p2P1P1/7q/1PP1B3/4KR1r b - - 0 1", "h1f1", 0);
    test_see("2r2r1k/6bp/p7/2q2p1Q/3PpP2/1B6/P5PP/2RR3K b - - 0 1", "c5c1", 2 * ROOK_VALUE - QUEEN_VALUE);
    test_see("r2qk1nr/pp2ppbp/2b3p1/2p1p3/8/2N2N2/PPPP1PPP/R1BQR1K1 w kq - 0 1", "f3e5", PAWN_VALUE);
    test_see("6r1/4kq2/b2p1p2/p1pPb3/p1P2B1Q/2P4P/2B1R1P1/6K1 w - - 0 1", "f4e5", 0);
    test_see("3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R4B/PQ3P1P/3R2K1 w - h6 0 1", "g5h6", 0);
    test_see("3q2nk/pb1r1p2/np6/3P2Pp/2p1P3/2R1B2B/PQ3P1P/3R2K1 w - h6 0 1", "g5h6", PAWN_VALUE);
    test_see("2r4r/1P4pk/p2p1b1p/7n/BB3p2/2R2p2/P1P2P2/4RK2 w - - 0 1", "c3c8", ROOK_VALUE);
//    test_see("2r5/1P4pk/p2p1b1p/5b1n/BB3p2/2R2p2/P1P2P2/4RK2 w - - 0 1", "c3c8", ROOK_VALUE);
    test_see("2r4k/2r4p/p7/2b2p1b/4pP2/1BR5/P1R3PP/2Q4K w - - 0 1", "c3c5", BISHOP_VALUE);
    test_see("8/pp6/2pkp3/4bp2/2R3b1/2P5/PP4B1/1K6 w - - 0 1", "g2c6", PAWN_VALUE - BISHOP_VALUE);
    test_see("4q3/1p1pr1k1/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - - 0 1", "e6e4", PAWN_VALUE - ROOK_VALUE);
    test_see("4q3/1p1pr1kb/1B2rp2/6p1/p3PP2/P3R1P1/1P2R1K1/4Q3 b - - 0 1", "h7e4", PAWN_VALUE);

    perft_summary_tests();

    std::cout << std::endl;
}
