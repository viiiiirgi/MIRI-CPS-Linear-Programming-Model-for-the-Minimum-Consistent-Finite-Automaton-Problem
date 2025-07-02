## Minimum Consistent Finite Automaton Solver ##

This program solves the Minimum Consistent Finite Automaton problem using Linear Programming with the CPLEX library in C++. It computes the smallest deterministic finite automaton (DFA) that:
- Accepts a given set of binary sequences
- Rejects another set of binary sequences
- Ensures full consistency with the input

## Requirements
- Linux OS
- `g++` with C++17 support
- IBM ILOG CPLEX Studio
- Graphviz (optional, used by the checker tool)

## Installation
Install dependencies on Ubuntu/Debian:

sudo apt update
sudo apt install g++ libgecode-dev graphviz


## Files
- `main.cpp`  Main constraint solver
- `Makefile` Used to compile `main.cpp`.
- `checker.cc`  Provided checker to verify DFA correctness and generate diagrams
- `runner.sh`  Optional script to batch process all instances
- `instances/`  Folder containing test instance `.inp` files
- `out/` The `runner.sh` script will create this folder to store output files.

## Compile the solver
Navigate to the directory containing `main.cpp` and `Makefile` (src), then run:
make



## Solve a single instance with a 60-second timeout:
timeout 60 ./main < instances/example.inp > out/example.out

## Batch Execution (Optional)
To run the solver and checker on all input files, use the `runner.sh` script. It will:
- Apply a timeout
- Write results to `out/`
- Validate outputs using the checker

### To use:
chmod +x runner.sh
./runner.sh


