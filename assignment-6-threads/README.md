# Assignment 6 — Multithreaded Bank

Stanford CS107 | Spring 2008

## Overview

A single-threaded bank simulation retrofitted to support multiple concurrent worker
threads using POSIX threads (`pthreads`). The goal is to eliminate race conditions and
deadlocks while maintaining maximum possible concurrency.

## Usage

```bash
make
./bankdriver -w4 -t1
```

Run all 7 test cases:

```bash
for i in 1 2 3 4 5 6 7; do ./bankdriver -w4 -t$i; done
```

## Flags

| Flag | Description |
|---|---|
| `-w N` | Number of worker threads |
| `-t N` | Test case number (1–7) |
| `-y` | Enable thread yielding to expose race conditions (5% yield rate) |
| `-y50` | Yield on 50% of calls |
| `-r` | Fewer operations — useful with Helgrind |
| `-d xyz` | Enable debug output for flags x, y, z |
| `-h` | Show all options |

## Testing

```bash
# Basic test
./bankdriver -w4 -t1

# With yielding to expose race conditions
./bankdriver -w4 -t1 -y

# With Helgrind to detect race conditions
valgrind --tool=helgrind ./bankdriver -r -t1 -w4
```

## How It Works

The bank is divided into branches, each containing accounts. Worker threads process
four types of requests concurrently: deposits/withdrawals, transfers, branch balance
queries, and bank balance queries.

### Locking Discipline

- Each **account** has its own lock — transfers within a branch proceed concurrently
- Each **branch** has its own lock — branches operate fully independently
- **`Report_DoReport()`** uses a condition variable barrier — all workers must finish
  the day before any proceeds to the next

### Concurrency Guarantees

- Workers on different branches never block each other
- Transfers from A→B and C→D (same branch, disjoint accounts) run concurrently
- No partially-completed transaction is ever observable

## File Structure

| File | Description |
|---|---|
| `teller.c` | Processes deposit, withdrawal, transfer, balance commands |
| `report.c` | Nightly report generation with thread barrier |
| `bank.h/.c` | Bank and branch data structures |
| `account.h/.c` | Account data structure |
| `bankdriver.c` | Test harness — spawns worker threads |
| `discussion.txt` | Answers to design and race condition questions |

## Synchronization Primitives Used

```c
pthread_mutex_t lock;
pthread_mutex_init(&lock, NULL);
pthread_mutex_lock(&lock);
pthread_mutex_unlock(&lock);

pthread_cond_t cond;
pthread_cond_wait(&cond, &lock);
pthread_cond_signal(&cond);
pthread_cond_broadcast(&cond);
```

> Note: `pthread_barrier_*` routines are not used per assignment restrictions.
