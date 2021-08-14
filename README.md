# Bitboard-Chess

### **Features:**

Move Generation:
- Magic Bitboards

Search:
- Quiescence search
- Iterative deepening
- Aspiration Windows
- Null move pruning

Move Ordering:
- MVV/LVA
- Hash move

Evaluation:
- Piece-Square tables
- Mobility
- Rudimentary king safety
- Tapered evaluation

- Transposition Table
- Rudimentary Opening book


### **Requirements:**
- ~270 mb of memory
- POPCNT and LZCNT instructions
    - If your processor is from Intel or AMD and made during the last decade, you should be fine
    - Otherwise, build from source (unfortunately, there isn't a build system in place, so you'll need to find a way to compile SFML)


### **Future changes:**
- Graphical interface will be removed and engine will be converted to follow UCI protocol
    - Compiling cross platform and building from source with SFML is a tedious (although I could use CMake or something)
    - UCI needs to be implemented for engine vs engine testing anyway
    - I don't want to keep adding features to the GUI (i.e. time control)
    - The GUI code is objectively bad and I am not particularly proud of it
- PV search
- Pawn structure
- Futility pruning
- More move ordering heuristics



