# Systems Programming: File I/O & Optimization

![Language](https://img.shields.io/badge/language-C11-blue)
![Platform](https://img.shields.io/badge/platform-Linux%20%2F%20POSIX-lightgrey)

A collection of high-performance C system utilities developed for the "Operating Systems" course at Osnabrück University. This project demonstrates deep understanding of Linux system calls, memory management, and algorithmic optimization.

## 🚀 Project Overview

The project consists of two distinct modules focusing on different aspects of systems programming:

1.  **File Copy Benchmarking**: A tool to compare different kernel-level strategies for copying data.
2.  **High-Performance Parser**: A highly optimized character frequency analyzer designed to process large datasets under strict time constraints.

## 🛠 Module 1: Copyfile (I/O Benchmarking)

This module implements and benchmarks three different methods for copying files on a Linux system to understand the overhead of context switches and memory buffers.

### Implemented Strategies
* **Buffered I/O**: Standard `read(2)` and `write(2)` loop using a user-space malloc'd buffer.
* **Memory Mapping**: Uses `mmap(2)` to map the source file into virtual memory, allowing direct memory-to-file writes via `write(2)`.
* **Zero-Copy**: Uses `sendfile(2)` to transfer data directly between file descriptors within the kernel, avoiding user-space data copying entirely.

### Usage
The program measures the execution time (in milliseconds) for each method, including resource allocation overhead.

```bash
make
./copyfile <mode> <source_file> <destination_file>
```

## Performance Comparison

---

count_naive

Time: 7049479 us

---

count array[512] * 8byte

Time: 1732285 us

---

count array[512] * 8byte + 1 producer & 1 consumer

Time: 1953109 us

--- 

count array[512] * 8byte + 1 producer & n consumer

Time: 972716 us

--- 
