# Assignment 2 — Six Degrees of Kevin Bacon

Stanford CS107 | Spring 2008

## Overview

An actor connection finder based on the "Six Degrees of Kevin Bacon" game. Given two actors, the program finds the shortest chain of co-starring relationships between them using BFS over a binary-encoded movie database.

## Usage

```bash
make
./six-degrees
```

## How It Works

### Task I — `imdb` class
Parses two binary data files (`actorFile`, `movieFile`) using raw pointer arithmetic and binary search over offset arrays. Implements two methods:
- `getCredits(actor, films)` — returns all films an actor appeared in
- `getCast(film, actors)` — returns all actors in a given film

### Task II — BFS path search
Finds the shortest path between two actors using breadth-first search:
- Maintains a queue of partial paths
- Expands each path by looking up the last actor's films, then each film's cast
- Stops when the target actor is found, or path length exceeds 6

## File Structure

| File | Description |
|---|---|
| `imdb.cc` | Core binary data parsing — `getCredits` and `getCast` |
| `imdb.h` | `imdb` class interface |
| `imdb-utils.h` | `film` struct with `operator<` and `operator==` |
| `six-degrees.cc` | BFS search logic |
| `path.h/.cc` | `path` class for building actor chains |

## Key Concepts

- **Actor record** — name (null-padded to even length) + film count (short) + array of offsets into `movieFile`
- **Movie record** — title + year as `year - 1900` (1 byte) + cast count (short) + array of offsets into `actorFile`
- **Offset array** — enables binary search over variable-length records
- **BFS** — guarantees the shortest path; paths of length 7+ are treated as no connection

## Compilation
```sh
make
```

## Memory check
```sh
./six-degrees-checker64 ./six-degrees
# check for memory leaks
./six-degrees-checker64 ./six-degrees -m
```
