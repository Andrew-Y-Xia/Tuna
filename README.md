# TUNA Chess Engine

Tuna is a UCI-compatible chess engine featuring a **neural network evaluation function (NNUE)** with **incremental accumulator updates**, combined with a sophisticated alpha-beta search framework.

**Estimated Strength**: 2950 Elo (±30)

## Architecture

### Neural Network Evaluation (NNUE)
- **Architecture**: (768 → 128)×2 → 1 (dual-perspective with SCReLU activations)
- **Input Layer**: 768 piece-centric features (standard Chess768 encoding)
- **Quantization**: int16 weights/biases with quantization scales (QA=255, QB=64)
- **Incremental Updates**: Accumulator caching with perspective-based evaluation, eliminates full network recomputation on most moves
- **Network Embedding**: Weights compiled into binary for zero external dependencies
- **Scale**: 400 centipawns per output unit

### Search Engine
**Core Algorithm**: Alpha-Beta search with Principal Variation Search (PVS)

**Depth Enhancement**:
- Late Move Reduction (LMR) with logarithmic reduction formula
- Futility Pruning (depth-scaled margins)
- Reverse Futility Pruning (Static Null Move Pruning)
- Null Move Pruning with configurable reduction
- Check Extensions (limited to 5 plies)
- Internal Iterative Reductions (IIR)
- Quiescence Search with delta pruning

**Move Selection & Ordering**:
- Static Exchange Evaluation (SEE) for capture scoring
- MVV/LVA (Most Valuable Victim / Least Valuable Attacker)
- Transposition Table hash move ordering
- Killer Move Heuristic (2 killers per ply)
- History Heuristic with scaling

**Board Representation & Move Generation**:
- Magic Bitboards for sliding piece attacks (O(1) lookup)
- Zobrist hashing for transposition table keys
- Incremental Zobrist updates
- Templated move generation (ALL_MOVES / CAPTURES_ONLY) with compile-time specialization

**Transposition Table**:
- Clustered design (4-entry buckets per index, cache-optimal on most modern machines)
- Upper/lower bound scoring with depth-relative entries (2^22 entries)
- Age-based replacement policy

### Search Parameters & Tuning
All search constants are tunable:
- LMR divisor and offset
- Futility/Reverse Futility margins
- Null move reduction depth
- Aspiration window initial width and growth factor
- Extension limits and IIR thresholds
- Parameters configurable at runtime without recompilation
- Better tuning framework in the future!

### Additional Features
- **Opening Book Support**: Pre-loaded opening lines (disabled by default)
- **Time Management**: Configurable time controls (constant time / infinite)
- **Multi-threaded Support**: Thread-safe infrastructure with safe queues
- **Build Optimization**: Many compile-time optimizations, template metaprogramming
- **UCI Compatibility**: Full protocol support via command queue system

## Building

### Requirements
- C++14 or later
- Highly preferred:
    - POPCNT and LZCNT CPU instructions (BMI2)
    - AVX512 instructions
- CMake 3.20+
- Python 3.x (for network embedding script)

### Unix-like Systems
```bash
cmake . -DCMAKE_BUILD_TYPE=Release
make
```

The build system automatically embeds the NNUE network weights into the executable, generating `NNUE_embedded.hpp` at build time.

## Testing

Test suite could be more thorough. Currently validates:
- **Perft Verification**: Move generation tested against known perft results
- **NNUE Incremental Updates**: Verifies incremental accumulator matches from-scratch evaluation at every node
- **Tuning Parameter System**: Tests parameter loading/saving and runtime modification
- **Move Generation**: Tests capture-only and all-move generation paths

## Training & Development

The `NNUE/` subrepository contains the training pipeline:
- **Dataset**: ~3.5 million positions with evaluations and game outcomes
    - Generated via self-play
- **Framework**: [bullet](https://github.com/jw1912/bullet) Rust trainer with CUDA/HIP/CPU support
- **Format**: FEN + centipawn score + result
- Network checkpoints stored with quantization applied
- Current nets trained on lc0 data.
    - One day when I have enough positions I'll train the nets on my own games

## Memory & Performance
- **Memory Usage**: ~270 MB (primarily transposition table)
- **NNUE Evaluation**: Cached incremental updates for sub-microsecond typical case
- **Bitboard Operations**: Highly perfomant move-gen: 50 million nps on perft

## Acknowledgements

The chess programming community has provided invaluable resources that made this project possible:

- The [Chess Programming Wiki](https://www.chessprogramming.org/) for comprehensive technical documentation on search algorithms, move generation, and evaluation techniques
- [Stockfish](https://stockfishchess.org) and [Crafty](https://craftychess.com) for demonstrating advanced engine architecture and optimization techniques
- [Leela Chess Zero](https://lczero.org/) for training data and game datasets
- The broader open-source chess engine community for sharing knowledge on NNUE networks, tuning methodologies, and performance analysis
- [bullet](https://github.com/jw1912/bullet) for an excellent neural network training framework
