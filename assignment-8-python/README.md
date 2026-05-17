# Assignment 8 — DNA Alignment

Stanford CS107 | Spring 2008

## Overview

A DNA sequence alignment program written in Python. Uses recursive dynamic
programming with memoization to find the optimal alignment of two DNA strands
and generate the full alignment with match/mismatch scores.

## Usage

```bash
# Python 2.4
python solution.pyc

# Python 2.7
python solution.py
```

## Testing

The `test` function validates your alignment result. Pass all five outputs to it:

```python
test(score, plusScores, minusScores, strandAligned1, strandAligned2)
```

| Parameter | Type | Example |
|---|---|---|
| `score` | `int` | `-6` |
| `plusScores` | `string` | `" 1 1 1"` |
| `minusScores` | `string` | `"22 111 11 "` |
| `strandAligned1` | `string` | `" CAAGTCGC"` |
| `strandAligned2` | `string` | `"ATCCCATTAC"` |

> All string parameters must be the same length — pad with trailing spaces if needed.

## How It Works

1. `findOptimalAlignment(strand1, strand2, cache)` recursively aligns two strands
2. At each step it tries three options: match/mismatch both leading chars, skip from strand1, skip from strand2
3. Returns a dictionary with keys `"strand1"`, `"strand2"`, and `"score"`
4. **Memoization** — results are cached in a shared dictionary passed through recursion, reducing exponential time to polynomial

## Task Summary

- Extend `findOptimalAlignment` to return the full alignment strings alongside the score
- Add memoization to handle strands up to length 60 within 10 seconds, and length 200 within ~2 minutes
- Keep the `while` loop in `main` and pass every result to `test`
