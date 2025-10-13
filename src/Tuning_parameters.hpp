//
//  Tuning_parameters.hpp
//  Tuna Chess Engine
//
//  Texel's Tuning Method - Parameter Registry System
//  Phase 1: Search Constants Refactoring
//

#ifndef TUNING_PARAMETERS_HPP
#define TUNING_PARAMETERS_HPP

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <iostream>
#include <algorithm>
#include "depend.hpp"

// Tunable parameter with metadata
struct TunableParam {
    std::string name;
    int* value_ptr;         // Pointer to actual parameter
    int min_value;          // Minimum allowed value
    int max_value;          // Maximum allowed value
    int initial_value;      // Starting value
    
    TunableParam(const std::string& n, int* ptr, int min_val, int max_val)
        : name(n), value_ptr(ptr), min_value(min_val), 
          max_value(max_val), initial_value(*ptr) {}
    
    int get() const { return *value_ptr; }
    
    // Set value with bounds checking
    void set(int val) {
        *value_ptr = std::max(min_value, std::min(max_value, val));
    }
    
    void reset() { *value_ptr = initial_value; }
};

// Global tuning parameter registry for search constants
class TuningParameters {
public:
    double lmr_base_divisor = 2.5;           // Base divisor in log formula
    double lmr_offset = 0.75;                // Offset added to reduction
    
    int futility_margin_multiplier = 82;     // Base: depth * PAWN_VALUE
    int futility_margin_base = 82;           // Additional constant margin
    
    int rfp_margin_multiplier = 123;         // Base: depth * 1.5 * PAWN_VALUE
    
    int null_move_reduction = 2;             // R value for null move pruning
    
    int aspiration_window_initial = 25;      // Initial window size
    int aspiration_window_multiplier = 4;    // Growth factor on fail
    
    int check_extension_limit = 5;           // Max plies to extend in check
    
    int iir_depth_threshold = 4;             // Minimum depth for IIR
    int iir_reduction = 1;                   // Depth reduction when no hash move
    
    void init_registry();
    
    std::vector<TunableParam>& get_params() { return params; }
    
    void save_to_file(const std::string& filename);
    void load_from_file(const std::string& filename);
    
    void print_params() const;
    
    // Get singleton instance
    static TuningParameters& instance() {
        static TuningParameters inst;
        return inst;
    }

private:
    std::vector<TunableParam> params;
    
    // Register a parameter for tuning
    void register_param(const std::string& name, int* ptr, 
                       int min_val, int max_val) {
        params.emplace_back(name, ptr, min_val, max_val);
    }
    
    // Private constructor for singleton
    TuningParameters() {}
};

#endif // TUNING_PARAMETERS_HPP
