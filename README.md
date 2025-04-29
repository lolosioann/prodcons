# Producer - Consumer

This repository contains a modified implementation of the classical **Producer–Consumer** problem, developed as part of the *Embedded Real-Time Systems* course at the School of Electrical and Computer Engineering, Aristotle University of Thessaloniki (ECE AUTH), Spring 2025.

## Overview

In this implementation, multiple producer and consumer threads coordinate through a shared bounded buffer. Each producer generates `workFunction` structures that encapsulate a function pointer and a list of arguments. These structures are inserted into a queue, from which consumers retrieve and execute them.

This version extends the standard model by introducing:
- Dynamic execution of tasks via function pointers
- Thread-safe timing mechanisms to measure queue wait times
- Atomic variables for synchronization control
- Pthread-based multithreading

## Architecture
- **Producers**: Generate `workFunction` structures, each containing:
    - A pointer to a computation routine (in our case a sine function)
    - Associated input arguments
- **Consumers**: Retrieve these structures, execute the pointed-to function, and discard results.
- **Shared Buffer**: Acts as the communication medium (circular queue with mutex + condition variable support).
- **Timing System**:
    - Two `timeQueue` structures record enqueue and dequeue times.
    - Used to compute the average waiting time in the queue.
- **Control Flags**: An `atomic_int stop_flag` ensures all threads can gracefully terminate.

## Compilation

To build the project:
```bash
make
```

This will generate the bin/pc executable.

To clean up object files and binaries:

```bash
make clean
```

## Execution

```bash
./bin/pc
```

The number of producer and consumer threads, as well as buffer sizes and loop iterations, can be configured by modifying the macro definitions in `globals.h`.

## Output
Retrievals from an empty queue and insertions to a full queue get preinted to `stdout`.
The average queue wait time is displayed at the end of the run.


## Dependencies
- POSIX threads (pthread)
- C11 atomics (stdatomic.h)
- Standard C libraries
