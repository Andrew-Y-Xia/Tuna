#include "depend.hpp"

#include "ResourcePath.hpp"


#include "Data_structs.hpp"
#include "Board.hpp"
#include "Utility.hpp"
#include "Search.hpp"


// Global vars:
sf::Texture textures[13];
std::forward_list<sf::Sprite> sprites, promotion_sprites_white, promotion_sprites_black;
int incre8[8];
double s_timer;


int main() {
    Bitboard::print_BB(eighth_rank);
    
    return 0;
}
