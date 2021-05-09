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
    
    Board board("r3k2r/p1ppqpb1/Bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPB1PPP/R3K2R b KQkq - 0 0");
    
    std::cout << '\n';
    std::vector<Move> moves;
    moves.reserve(256);
    board.generate_moves(moves);
    
//    std::cout << '\n' << moves.size() << '\n';
//
    Search search(board);

    auto t1 = std::chrono::high_resolution_clock::now();
    
    /*
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
     */
     
    

//    std::cout << board.Perft(8);
    
    print_move(search.find_best_move(4), true);
    

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = t2 - t1;
    
    std::cout << "\nTime: " << ms_double.count() << "ms\n";
    
//    board.print_board();
    
    

    return 0;
}
