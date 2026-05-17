# Assignment 7 — Where Am I?

Stanford CS107 | Spring 2008

## Overview

A location estimator written in Scheme. Given a star map and approximate distances
to several stars, the program determines your position in 2D space by finding where
circles around stars intersect and clustering the intersection points.

## Usage

```scheme
kawa
(load "where-am-i.scm")
```

## Testing

Run all tests at once:

```scheme
(load "where-am-i.scm")
(test-all)
```

Run individual tests:

```scheme
(test-intersection-points)
(test-distance-product)
(test-rate-points)
(test-sort-points)
(test-clumped-points)
(test-average-point)
(test-best-estimate)
(test-where-am-i)
```

## How It Works

1. **Generate guesses** — pair each observed distance with each possible star (`all-guesses`)
2. **Intersect circles** — for each guess, find all circle intersection points (`intersection-points`)
3. **Rate points** — score each point by its distance-product from all others (`rate-points`)
4. **Sort and clump** — sort by rating, take the lowest-rated half (`clumped-points`)
5. **Average** — average the clumped points into a single rated estimate (`average-point`)
6. **Pick best** — sort all guesses by their estimate rating; the lowest rating is your location (`where-am-i`)

## Function Summary

| Function | Description |
|---|---|
| `intersection-points` | All pairwise intersection points of a list of circles |
| `distance-product` | Product of distances from a point to all others |
| `rate-points` | Annotates each point with its distance-product rating |
| `sort-points` | Sorts rated points in ascending order |
| `clumped-points` | Returns the lowest-rated half of points (the clump) |
| `average-point` | Averages a list of points into one rated point |
| `best-estimate` | Best location estimate from a single guess |
| `where-am-i` | Finds all estimates and returns them sorted by likelihood |

## Key Concepts

- **Circle** — defined by center (star position) and radius (observed distance)
- **Clumping** — correct guesses produce intersection points clustered in one area
- **Distance-product** — low value means a point is close to all others (in the clump)
- **Noise tolerance** — distances are approximate, so circles don't intersect exactly
