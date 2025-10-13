//
//  Tuning_parameters.cpp
//  Tuna Chess Engine
//
//

#include "Tuning_parameters.hpp"
#include <sstream>
#include <iomanip>

// ==================== TuningParameters Implementation ====================

void TuningParameters::init_registry() {
    params.clear();
    
    // Note: We don't register double parameters in the tuning system yet
    // The LMR formula parameters (lmr_base_divisor, lmr_offset) stay as doubles
    
    register_param("futility_margin_multiplier", &futility_margin_multiplier, 50, 150);
    register_param("futility_margin_base", &futility_margin_base, 50, 150);
    
    register_param("rfp_margin_multiplier", &rfp_margin_multiplier, 80, 200);
    
    register_param("big_delta", &big_delta, 800, 1200);
    
    register_param("null_move_reduction", &null_move_reduction, 1, 4);
    
    register_param("aspiration_window_initial", &aspiration_window_initial, 10, 50);
    register_param("aspiration_window_multiplier", &aspiration_window_multiplier, 2, 6);
    
    register_param("check_extension_limit", &check_extension_limit, 3, 10);
    
    register_param("iir_depth_threshold", &iir_depth_threshold, 3, 6);
    register_param("iir_reduction", &iir_reduction, 1, 2);
    
    std::cout << "TuningParameters: Registered " << params.size() << " tunable search parameters" << std::endl;
}

void TuningParameters::save_to_file(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for writing: " << filename << std::endl;
        return;
    }
    
    file << "# Tuna Tuning Parameters (Search)\n";
    file << "# Format: parameter_name = value\n";
    file << "# Note: Evaluation will be handled by NNUE in the future\n\n";
    
    // Save non-registered parameters (doubles)
    file << "lmr_base_divisor = " << lmr_base_divisor << "\n";
    file << "lmr_offset = " << lmr_offset << "\n\n";
    
    // Save registered parameters
    for (const auto& param : params) {
        file << param.name << " = " << param.get() << "\n";
    }
    
    file.close();
    std::cout << "TuningParameters: Saved " << params.size() << " parameters to " << filename << std::endl;
}

void TuningParameters::load_from_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file for reading: " << filename << std::endl;
        return;
    }
    
    std::string line;
    int loaded_count = 0;
    
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;
        
        // Parse "name = value"
        size_t eq_pos = line.find('=');
        if (eq_pos == std::string::npos) continue;
        
        std::string name = line.substr(0, eq_pos);
        std::string value_str = line.substr(eq_pos + 1);
        
        // Trim whitespace
        name.erase(0, name.find_first_not_of(" \t"));
        name.erase(name.find_last_not_of(" \t") + 1);
        value_str.erase(0, value_str.find_first_not_of(" \t"));
        value_str.erase(value_str.find_last_not_of(" \t") + 1);
        
        // Try to parse as double first (for LMR parameters)
        if (name == "lmr_base_divisor") {
            lmr_base_divisor = std::stod(value_str);
            loaded_count++;
            continue;
        }
        if (name == "lmr_offset") {
            lmr_offset = std::stod(value_str);
            loaded_count++;
            continue;
        }
        
        // Try to find and set parameter
        int value = std::stoi(value_str);
        for (auto& param : params) {
            if (param.name == name) {
                param.set(value);
                loaded_count++;
                break;
            }
        }
    }
    
    file.close();
    std::cout << "TuningParameters: Loaded " << loaded_count << " parameters from " << filename << std::endl;
}

void TuningParameters::print_params() const {
    std::cout << "\n=== Tuning Parameters (Search) ===\n";
    std::cout << "lmr_base_divisor: " << lmr_base_divisor << "\n";
    std::cout << "lmr_offset: " << lmr_offset << "\n";
    
    for (const auto& param : params) {
        std::cout << std::setw(35) << std::left << param.name 
                  << " = " << std::setw(6) << std::right << param.get()
                  << "  [" << param.min_value << ", " << param.max_value << "]\n";
    }
    std::cout << "==================================\n\n";
}
