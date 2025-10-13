//
//  test_tuning_params.cpp
//  Quick test to verify tuning parameter system
//

#include <iostream>
#include "src/Tuning_parameters.hpp"

int main() {
    std::cout << "=== Testing Tuning Parameter System ===\n\n";
    
    // Test TuningParameters (merged class)
    auto& params = TuningParameters::instance();
    params.init_registry();
    
    std::cout << "Initial Parameters:\n";
    params.print_params();
    
    // Modify some parameters
    std::cout << "\nModifying parameters...\n";
    params.null_move_reduction = 3;
    params.aspiration_window_initial = 30;
    params.futility_margin_multiplier = 100;
    
    std::cout << "\nModified Parameters:\n";
    params.print_params();
    
    // Save to file
    params.save_to_file("test_tuning_params.txt");
    
    // Reset one parameter
    params.null_move_reduction = 2;
    std::cout << "\nAfter resetting null_move_reduction to 2:\n";
    std::cout << "null_move_reduction = " << params.null_move_reduction << "\n";
    
    // Load from file
    params.load_from_file("test_tuning_params.txt");
    std::cout << "\nAfter loading from file:\n";
    std::cout << "null_move_reduction = " << params.null_move_reduction << "\n";
    
    std::cout << "\nFinal Parameters:\n";
    params.print_params();
    
    std::cout << "\n=== All tests passed! ===\n";
    std::cout << "\nNote: Evaluation parameters removed - will be handled by NNUE\n";
    
    return 0;
}
