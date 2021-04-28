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
    
    Board board("rnbqkbnr/2p3pp/1p2p3/p2p1p2/4P2P/5Q1R/PPPP1PP1/RNB1KBN1 w Qkq f6 0 1");
//    board.print_board();
    
    std::cout << '\n';
    std::vector<Move> moves;
    moves.reserve(256);
    board.generate_moves(moves);
    
    std::cout << '\n' << moves.size() << '\n';
    
    
    for (auto it = moves.begin(); it != moves.end(); ++it) {
        print_move(*it, true);
        std::cout << ": 1\n";
    }
    
    
    

    return 0;
}
