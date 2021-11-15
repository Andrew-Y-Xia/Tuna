# Bitboard-Chess

Bitboard Chess is an UCI-compatible Chess Engine. It uses a search centered around the Alpha-Beta algorithm and a hand-crafted (and minimal) positional evaluation based off material balance in order to play a decent game of chess.

### **Compiling from source**
#### Unix-like
```
cmake ./CMakeLists.txt -DCMAKE_BUILD_TYPE=Release
make
```


### **Features:**

Move Generation:
- Magic Bitboards

Search:
- Principal Variation Search
- Iterative Deepening
- Aspiration Windows
- Null Move Pruning
- Late Move Reductions
- Check Extensions
- Quiescence Search

Move Ordering:
- Static Exchange Evaluation
- MVV/LVA
- Hash moves
- Killer moves
- History heuristic

Evaluation:
- Piece-Square tables
- Mobility
- Rudimentary King Safety
- Tapered Eval

Miscellaneous:
- Transposition Table
- Opening book
- UCI Compatibility


### **Requirements:**
- ~270 mb of memory
- POPCNT and LZCNT instructions
    - If not, build from source


### **Future changes:**
- NNUE?

## Acknowledgements
This project would not have been possible without the [Chess Programming Wiki](https://www.chessprogramming.org/Main_Page). Additional thanks to open source engines such as [Crafty](https://craftychess.com) and [Stockfish](https://stockfishchess.org).
