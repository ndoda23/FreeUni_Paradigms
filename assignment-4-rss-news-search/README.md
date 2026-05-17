# Assignment 4 — RSS News Feed Aggregator

Stanford CS107 | Spring 2008

## Overview

A news search engine that fetches RSS feeds, indexes article content by word frequency,
and lets the user query for articles by keyword. Built on top of the custom `vector`
and `hashset` from Assignment 3.

## Setup

Install required packages:

```bash
dpkg --add-architecture i386
apt update
apt install gcc make
apt install libcurl4-openssl-dev:i386
apt install libnsl-dev:i386
apt install gcc-multilib
```

## Usage

```bash
make
./rss-news-search
```

The first `make` run automatically downloads the required data files into `data/`.
To re-download them:

```bash
rm -rf data/
make data
```

## Testing

```bash
./assn-4-checker-64 ./rss-news-search
./assn-4-checker-64 ./rss-news-search -m
```

## How It Works

1. Fetches RSS feeds from major news sites
2. Parses each article into individual words using `streamtokenizer`
3. Filters out stop words (e.g. "the", "without")
4. Indexes remaining words into a `hashset` mapping keywords → articles + frequency
5. Accepts user queries and returns top 10 matching articles sorted by word count

## File Structure

| File | Description |
|---|---|
| `rss-news-search.c` | Main logic — indexing and querying |
| `streamtokenizer.h/.c` | Breaks web page content into word tokens |
| `urlconnection.h/.c` | Fetches content from URLs |
| `data/stop-words.txt` | List of common words excluded from index |

## Hash Function

Uses a string hash based on linear congruence (adapted from Eric Roberts' *The Art and Science of C*). Case-insensitive — "Paris" and "paris" hash to the same bucket.

```c
static const signed long kHashMultiplier = -1664117991L;

static int StringHash(const char *s, int numBuckets) {
    int i;
    unsigned long hashcode = 0;
    for (i = 0; i < strlen(s); i++)
        hashcode = hashcode * kHashMultiplier + tolower(s[i]);
    return hashcode % numBuckets;
}
```

## Key Concepts

- **Stop words** — common words loaded into a `hashset` and ignored during indexing
- **Index** — a `hashset` mapping each word to a list of articles and occurrence counts
- **Deduplication** — same article not indexed twice (matched by URL or title+server)
- **Case-insensitive** — queries and indexing use `tolower` and `strcasecmp`
- **Buckets** — large prime number (1009 or 10007) for uniform hash distribution
