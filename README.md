*This project has been created as part of the 42 curriculum by guifouqu.*

## Overview
Codexion is a concurrent programming project that simulates multiple coders competing for shared USB dongles to complete their tasks (compiling, debugging, and refactoring). The project focuses on mastering resource synchronization, avoiding deadlocks, and implementing efficient scheduling in a multi-threaded environment.

## Features
- **Multi-threaded Simulation:** Each coder runs in a separate thread.
- **Resource Management:** Precise handling of shared resources (dongles) using mutexes.
- **Scheduling Strategies:**
    - **FIFO (First-In-First-Out):** Standard queue-based order.
    - **EDF (Earliest Deadline First):** Priority-based scheduling using a Min-Heap, including a custom tie-breaker favoring higher Coder IDs.
- **Burnout Detection:** Monitoring system to track coder inactivity and prevent simulation failure.

## Instructions

### Prerequisites
- A C compiler (gcc/clang)
- Make
- pthread library

### Compilation
The project uses a standard Makefile:
```bash
make

### Execution
Run the simulation with the following arguments:
````
./codexion <nb_coders><time_to_burnout><time_to_compile><time_to_debug><time_to_refactor><compliles_required><dongle_cooldown><scheduler_type>
```
*Example*:
./codexion 5 800 200 100 100 2 0 edf

### Technical Implementation
Deadlock Prevention:
To prevent circular wait conditions (deadlocks) when coders compete for the same dongles, we implement a strict resource hierarchy strategy:
	Every coder requires two specific dongles to perform their tasks.
	Our algorithm forces the acquisition of the dongle with the lowest ID first.
	By ensuring that every thread requests resources in the exact same relative order, we eliminate the possibility of a circular dependency where each thread holds one resource while waiting for another held by its neighbor.
	
### Thread-Safe Communication
Communication between the coders and the supervisor (the monitoring system) is achieved through several robust mechanisms:
	Shared Data Structure (t_data): The simulation state is protected by a global sim_mutex. This ensures that status checks (such as check_sim_over) or state updates are atomic and thread-safe.
	Resource-Specific Mutexes: Each individual dongle is protected by its own mutex, guaranteeing that access is strictly exclusive.
	Serialized Logging: A dedicated print_mutex is used for all console outputs, preventing interleaved or garbled messages from multiple threads.
	Synchronization: Threads use status flags and careful mutex management to ensure that when the supervisor detects a termination condition (burnout or completed quota), all threads are signaled to terminate cleanly.
	
### Algorithm Choices
	Min-Heap: Our EDF scheduler uses a Min-Heap for efficient priority management. This guarantees $O(\log n)$ complexity for insertions and removals.
	EDF Tie-breaker: To resolve equal deadlines, we implemented a custom comparison logic in our heap traversal (sift_up and sift_down), ensuring the coder with the higher ID is prioritized.

### AI Usage
Artifical Intelligence was used to assist in the development of this projet:
	Debugging & Synchronization: Assisting in identifying race conditions and refining the supervisor loop logic.
	Notions: Helping to find and understand differents notions about this project.
	Documentation: Structuring and translating the project README to English to align with explicit requirements.

Note on AI assistance:
	Everything implemented with the help of AI has been thoroughly understood, analyzed, and verified before being integrated into the project.