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
    
    Board board("r1bqkb1r/pPp3pp/5p2/1B2NnP1/8/P7/1Pp2P1P/RNBQK2R b KQkq - 0 1");
    board.print_board();
    
    std::cout << '\n';
    std::vector<Move> moves;
    moves.reserve(256);
    board.generate_moves(moves);
    
    
    for (auto it = moves.begin(); it != moves.end(); ++it) {
        print_move(*it);
    }
    

    return 0;
}
