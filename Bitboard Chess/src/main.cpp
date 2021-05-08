#include "depend.hpp"

#include "ResourcePath.hpp"


#include "Data_structs.hpp"
#include "Board.hpp"
#include "Utility.hpp"
#include "Search.hpp"
#include "Bitboard.hpp"
#include "Evaluation.hpp"


// Global vars:
//sf::Texture textures[13];
//std::forward_list<sf::Sprite> sprites, promotion_sprites_white, promotion_sprites_black;
//int incre8[8];
//double s_timer;



int main() {
    init_bitboard_utils();
    init_eval_utils();
    
    Board board("r1b1kb1r/pppp1ppp/5q2/4n3/3KP3/2N3PN/PPP4P/R1BQ1B1R b kq - 0 1");
    
    std::cout << '\n';
    std::vector<Move> moves;
    moves.reserve(256);
    board.generate_moves(moves);
    
//    std::cout << '\n' << moves.size() << '\n';
//
    Search search(board);

    auto t1 = std::chrono::high_resolution_clock::now();
    
    
    long nodes;
    for (auto it = moves.begin(); it != moves.end(); ++it) {
        long node;
        print_move(*it, true);
//        board.print_board();
        board.make_move(*it);
        node = board.Perft(4);
        std::cout << ": " << node << std::endl;
        nodes += node;
        board.unmake_move();
    }
    std::cout << "\n\nNodes searched: " << nodes;
     
    

//    std::cout << board.Perft(8);
    
//    print_move(search.find_best_move(5), true);
    

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = t2 - t1;
    
    std::cout << "\nTime: " << ms_double.count() << "ms\n";
    
//    board.print_board();
    
    

    return 0;
}
