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
    
    Board board("r1bqkbnr/pppp1ppp/2n5/4p3/4P3/5N2/PPPP1PPP/RNBQKB1R w KQkq - 0 1");
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
