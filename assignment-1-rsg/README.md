# Assignment 1 — Random Sentence Generator (RSG)

Stanford CS107 | Spring 2008

## Overview

A random sentence generator built on Context-Free Grammars (CFG). The program reads a grammar file, then recursively expands rules starting from `<start>`, printing three randomly generated sentences.


## How It Works

1. Parses a `.g` grammar file into a `map<string, Definition>`
2. Begins recursive expansion from `<start>`
3. For each non-terminal, randomly selects one of its productions
4. Collects terminals into a `vector<string>` and prints them

## File Structure

| File | Description |
|---|---|
| `rsg.cc` | Core logic — grammar expansion and output |
| `definition.h/.cc` | Models a non-terminal and its productions |
| `production.h/.cc` | Models a single production (sequence of tokens) |
| `random.h/.cc` | Random number utility |

## Key Concepts

- **Non-terminal** — a placeholder like `<verb>` that expands further
- **Terminal** — a literal word, no further expansion
- **Production** — one possible expansion of a non-terminal
- **Grammar** — full set of rules loaded from the `.g` file

## Notes

- Grammar files are assumed to be well-formatted
- If an expansion references an undefined non-terminal, the program exits via `assert`
- Three sentences are generated per run without re-reading the grammar

## Environment Setup


### Ubuntu
```sh
sudo apt-get install valgrind
```
### Arch
```sh
yay -S valgrind
```

## Compilation
```sh
make
```

## Run a Single Test
```sh
./rsg data/bionic.g
```

## Memory check
```sh
./rsgChecker32 ./rsg data/bionic.g
./rsgChecker64 ./rsg data/bionic.g
```

## Run All Tests
```sh
make test_all
``` 
