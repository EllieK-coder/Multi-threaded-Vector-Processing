# Multi-threaded-Vector-Processing
A high-performance C application that calculates the mean value of squares of a large vector (4M+ elements) utilizing parallel processing. The workload is evenly distributed among $t$ concurrently running POSIX Threads (pthreads).
Key Features
Dynamic Memory Allocation: Efficient handling of large arrays using malloc and free.

Thread Synchronization: Implementation of Mutexes (pthread_mutex_t) to eliminate Race Conditions during global variable updates in the Critical Section.

Performance Benchmarking: Comparative execution analysis between Windows (Dev-C++) and Linux (Bash/Kali) scheduling environments.

Performance Results (4,000,000 elements)

1 Thread: [X] clock cycles

2 Threads: [X] clock cycles

4 Threads: 1525 clock cycles (Linux Bash)

8 Threads: [X] clock cycles
