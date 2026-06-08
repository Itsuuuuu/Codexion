*This project has been created as part of the 42 curriculum by guifouqu.*

## Description
Codexion is a concurrent programming project that simulates multiple coders competing for shared USB dongles to complete their tasks (compiling, debugging, and refactoring). The project focuses on mastering resource synchronization, avoiding deadlocks, and implementing efficient scheduling in a multi-threaded environment.

**Key features:**
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
```bash
make
```

### Execution
```bash
./codexion <nb_coders> <time_to_burnout> <time_to_compile> <time_to_debug> <time_to_refactor> <compiles_required> <dongle_cooldown> <scheduler_type>
```
*Example:*
```bash
./codexion 5 800 200 100 100 2 0 edf
```

## Blocking cases handled

### Deadlock Prevention
To prevent circular wait conditions (deadlocks) when coders compete for the same dongles, we implement a strict resource hierarchy strategy:
- Every coder requires two specific dongles to perform their tasks.
- Our algorithm forces the acquisition of the dongle with the **lowest ID first**.
- By ensuring that every thread requests resources in the exact same relative order, we eliminate the possibility of a circular dependency where each thread holds one resource while waiting for another held by its neighbor.

### Starvation Prevention
- Under **EDF** scheduling, the coder with the earliest burnout deadline is always served first, guaranteeing that no coder starves as long as the parameters are feasible.
- The **FIFO** scheduler serves requests in strict arrival order, also preventing starvation.

### Cooldown Handling
After a dongle is released, it remains unavailable for `dongle_cooldown` milliseconds. This is tracked via a `cooldown_end` timestamp on each dongle, checked inside the acquisition loop before granting access.

### Precise Burnout Detection
The supervisor loop polls every 1ms under `sim_mutex`, checking `get_time_ms() - last_compile_start > time_to_burnout` for each coder. This guarantees the burnout log is printed within ~1ms of the actual burnout time, well within the 10ms requirement.

### Log Serialization
A dedicated `print_mutex` protects all calls to `printf`. Only one thread can write at a time, preventing interleaved or garbled output lines.

## Thread synchronization mechanisms

### `sim_mutex` (pthread_mutex_t)
Protects all shared simulation state: `simulation_over`, `threads_done`, `last_compile_start`, and `compiles_count`. Both the supervisor and coder threads access these fields exclusively under this mutex.

### Per-dongle mutex (pthread_mutex_t)
Each dongle has its own mutex protecting `is_available`, `cooldown_end`, and the priority heap. This guarantees exclusive access and prevents two coders from simultaneously claiming the same dongle.

### `print_mutex` (pthread_mutex_t)
Serializes all console output. Every call to `print_status` locks this mutex, ensuring log lines never interleave.

### Priority heap (custom)
Each dongle maintains a Min-Heap of waiting coders. Under FIFO, priority is the arrival timestamp. Under EDF, priority is `last_compile_start + time_to_burnout`. The heap is accessed exclusively under the dongle's mutex, preventing race conditions on the queue itself.

### Thread-safe termination
When the supervisor detects burnout or quota completion, it sets `simulation_over = 1` under `sim_mutex`. All threads check this flag regularly via `check_sim_over()`. After the supervisor loop exits, `pthread_cond_broadcast` is sent on every dongle to wake any blocked threads, allowing them to exit cleanly before `pthread_join`.

## Resources

### Documentation & references
- [POSIX Threads Programming — Lawrence Livermore National Laboratory](https://hpc-tutorials.llnl.gov/posix/)
- [The Little Book of Semaphores — Allen B. Downey](https://greenteapress.com/wp/semaphores/)
- [Dining Philosophers Problem — Wikipedia](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [Earliest Deadline First scheduling — Wikipedia](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)
- [Coffman conditions (deadlock) — Wikipedia](https://en.wikipedia.org/wiki/Deadlock#Coffman_conditions)
- `man pthread_mutex_init`, `man pthread_create`, `man gettimeofday`

### AI usage
Artificial Intelligence was used to assist in the development of this project:
- **Debugging & Synchronization:** Assisting in identifying race conditions and refining the supervisor loop logic.
- **Concepts:** Helping to find and understand notions related to concurrency, EDF scheduling, and heap structures.
- **Documentation:** Structuring and writing the README in English to align with project requirements.

*Everything implemented with the help of AI has been thoroughly understood, analyzed, and verified before being integrated into the project.*