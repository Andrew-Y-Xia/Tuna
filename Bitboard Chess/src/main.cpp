#include "depend.hpp"

#include "ResourcePath.hpp"


#include "Data_structs.hpp"
#include "Board.hpp"
#include "Utility.hpp"
#include "Search.hpp"
#include "Bitboard.hpp"


// Global vars:
//sf::Texture textures[13];
//std::forward_list<sf::Sprite> sprites, promotion_sprites_white, promotion_sprites_black;
//int incre8[8];
//double s_timer;



int main() {
    init_bitboard_utils();
    
    Board board("rnbqkbnr/ppp1pppp/8/3p4/Q7/P1P5/1P1PPPPP/RNB1KBNR b KQkq - 0 1");
//    board.print_board();
    
    std::cout << '\n';
    std::vector<Move> moves;
    moves.reserve(256);
    board.generate_moves(moves);
    
//    std::cout << '\n' << moves.size() << '\n';
//    
    
     
     
    
    auto t1 = std::chrono::high_resolution_clock::now();
    
    
    for (auto it = moves.begin(); it != moves.end(); ++it) {
        print_move(*it, true);
//        board.print_board();
        board.make_move(*it);
        std::cout << ": " << board.Perft(1) << std::endl;
        board.unmake_move();
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms_double = t2 - t1;
    
    std::cout << "\nTime: " << ms_double.count() << "ms\n";
    
//    board.print_board();
    

    
    

    return 0;
}
