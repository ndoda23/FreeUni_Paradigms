# Assignment 3 — Vector and Hashset

Stanford CS107 | Spring 2008

## Overview

Implementation of two generic container data structures in pure C — a dynamic array (`vector`) and a hash table (`hashset`) — without templates or STL. Uses raw memory management with `malloc`, `realloc`, `memcpy`, `memmove`, `qsort`, and `bsearch`.


## How It Works

### vector
A dynamically resizing array that stores elements of any type via `void *`. Pre-allocates memory in chunks of `initialAllocation` size and grows automatically when full.

| Function | Description |
|---|---|
| `VectorNew` | Initializes vector with element size and allocation chunk |
| `VectorAppend` | Copies element to end, reallocates if needed |
| `VectorInsert` | Inserts at position, shifts elements right with `memmove` |
| `VectorDelete` | Removes at position, shifts elements left with `memmove` |
| `VectorSearch` | Linear or binary search (`bsearch`) depending on `isSorted` |
| `VectorSort` | Sorts using `qsort` with client-supplied comparator |
| `VectorMap` | Iterates over all elements calling client function |

### hashset
A hash table built on top of `vector`. Each bucket is a `vector`; collisions are resolved by chaining. Client supplies hash function, compare function, and optional free function.

| Function | Description |
|---|---|
| `HashSetNew` | Initializes array of `numBuckets` vectors |
| `HashSetEnter` | Hashes element, inserts or replaces in bucket |
| `HashSetLookup` | Hashes key, searches bucket for match |
| `HashSetMap` | Iterates over all buckets and all elements |

## File Structure

| File | Description |
|---|---|
| `vector.c` | Full vector implementation |
| `vector.h` | Vector interface and type definitions |
| `hashset.c` | Full hashset implementation built on vector |
| `hashset.h` | Hashset interface and type definitions |
| `vector-test.c` | Vector test harness |
| `hashset-test.c` | Hashset test harness |

## Key Concepts

- **`void *`** — type-erased pointer enabling generic storage
- **`memcpy` vs `memmove`** — `memmove` used when source and destination overlap (shifts)
- **`realloc`** — grows allocated memory when vector is full
- **Chaining** — hash collisions stored in the same bucket's vector
- **Client-supplied functions** — comparator, hash, free functions passed as pointers

## Compilation
```sh
make
```


## Test
```sh
./vector-test
./hashset-test
```

`vector-test` and `hashset-test`  `sample-output-vector.txt` and `sample-output-hashset.txt` 
```sh
./vector-test | diff sample-output-vector.txt -
./hashset-test | diff sample-output-hashset.txt -
```

## Thesaurus
The thesaurus-lookup.c and streaktokenizer.c files, when
compiled against fully operational versions of vector and hashset,
contribute to an application called thesaurus-lookup, which is
this C-string intense application that loads a thesaurus into
a hashset (where synonym sets are stored in vectors).  You don't
need to do anything for this extra thesaurus-lookup application.
I just decided to include it with the starter files so you have
a much more sophisticated test application to exercise your implementations.
It also provides a good amount of sample code for you to read over
while working on next week's Assignment 4, which requires you
use the hashset and the vector to build an index of hundreds of online
news articles (with real networking!)
