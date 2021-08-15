#include "depend.hpp"



#include "Data_structs.hpp"
#include "Board.hpp"
#include "Utility.hpp"
#include "Search.hpp"
#include "Bitboard.hpp"
#include "Evaluation.hpp"
#include "Transposition_table.hpp"
#include "Opening_book.hpp"
#include "Engine.hpp"
#include "UCI.hpp"
#include "Thread.hpp"



int main() {
    
    init_bitboard_utils();
    init_eval_utils();
    init_ray_gen();
    init_zobrist_bitstrings();
    init_opening_book();
    
    std::cout << "Start\n";
    
    std::thread t(Engine::loop);
    UCI::loop();
    
    t.join();
}
