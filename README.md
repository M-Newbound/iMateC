# iMateC

A UCI-compatible chess engine written in C. Started as a learning project, ended up being a proper engine with magic bitboards, iterative deepening, and tapered evaluation. It plays decent chess and passes full perft validation through depth 4 on the standard test positions.

---

## Building

Requires CMake 3.10+ and a C11 compiler (gcc or clang both work).

```bash
cmake -S . -B build
cmake --build build
```

The engine binary ends up at `build/iMateC`. Defaults to Release mode so it runs at actual speed.

To run the tests:

```bash
cd build && ctest --output-on-failure
```

Or run them individually if you want to see the output:

```bash
./build/test_perft
./build/test_move_generation
```

---

## Running

```bash
./build/iMateC
```

Works with any UCI-compatible GUI (Arena, Cute Chess, etc). It also runs interactively from the terminal - type `help` to see all the commands.

### Example session

```
position startpos moves e2e4 e7e5
go depth 6
```

```
info depth 1 score cp 37 nodes 20 nps 200000 time 0 pv e2e4
info depth 2 score cp 0 nodes 73 nps 730000 time 0 pv e2e4 e7e5
...
bestmove g1f3
```

Other useful commands:

```
print board          -- show the current position
print moves e2       -- list legal moves from e2
status               -- check/checkmate/stalemate
move e2e4            -- apply a move interactively
position fen <fen>   -- load any FEN string
```

---

## Design

**Representation** - 64-bit bitboards, one per (color, piece) pair. The state struct is opaque; everything goes through accessor functions so the internal layout doesn't leak into move generation or search.

**Magic bitboards** - rooks and bishops use pre-computed attack tables with magic multipliers for O(1) lookup. Magics are found at startup with a fixed-seed XorShift PRNG, so the process is deterministic and takes well under a second.

**Move generation** - pseudo-legal moves are generated for each piece, then filtered by playing each move on a cloned board and checking if the moving side's king lands in check. Not the fastest approach (a proper pin/ray analysis would avoid the clones) but it's simple and correct, and the perft numbers back it up.

**Search** - negamax alpha-beta with iterative deepening. Captures are ordered by MVV-LVA (most valuable victim, least valuable attacker) before each search, which helps alpha-beta cut more branches. Time management reads `wtime`/`btime` from the UCI `go` command and allocates roughly `time_left / 20` per move, with a 50ms floor so depth 1 always finishes.

**Evaluation** - material + tapered piece-square tables. The taper blends between opening and endgame tables based on how much material is left on the board. Standard centipawn scale (pawn = 100). Checkmate is represented as ±FLT_MAX.

---

## Limitations

- No transposition table - probably the biggest remaining speed improvement
- No quiescence search - the engine can be tricked by tactics right at the search horizon (the horizon effect)
- No opening book
- Single-threaded

The move generator is correct - perft(4) on the starting position gives 197,281 nodes and Kiwipete depth 3 gives 97,862, both matching the reference values.

---

## Project structure

```
src/
  Main.c                   entry point
  IMate.h / IMate.c        main engine loop, regex-based command dispatch
  State/                   board representation, FEN parsing, move application
  Moves/                   move generation, magic bitboard tables
  Search/                  negamax alpha-beta with iterative deepening
  Evaluation/              static eval + piece-square tables
  Commands/                UCI and interactive command handlers
  Util/                    UCI move string parsing
tests/
  framework.h              minimal single-header test macros
  test_*.c                 unit tests and perft
```
