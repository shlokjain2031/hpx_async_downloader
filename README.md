# Async File Downloader using HPX

## Description
This project implements an **async file downloader** using **HPX**, an **asynchronous many-task runtime**.  
The code utilizes **HPX futures and threads** and custom coroutines to distribute computation efficiently across multiple cores.

### **Features**
- Uses **HPX futures and threads** (` hpx::async`) (`hpx::future`)
- Asynchronously reads URLs from a file, `urls.txt` using coroutines 
- Concurrently sends download requests through `SharedUrlQueue`
- Accepts **coroutine batch size** via command-line arguments.
- Outputs execution time for benchmarking along with speedup, throughput, efficiency and latency
---

## **Build & Run Instructions**
### **Dependencies**
- **HPX (C++ parallel runtime)**
- **CMake (Build system)**
- **vcpkg (for dependency management, optional)**

### **Installation**
- Install HPX using a package manager
- Clone the repository
- Build the project using CMake
- Run the program with the following command

  `./parallel_matrix_multi --n 3 --m 2 --k 2 --s 1000 --l 0 --u 10000`
- Also generate benchmarks by running `/benchmarks/run_benchmarks.sh` using the following command
```
chmod +x run_benchmarks.sh
./run_benchmarks.sh  
```

### **Benchmark Results**

### **Implementation Details**
- Implemented SharedUrlQueue using std::mutex, std::condition_variable, and atomic done_ flag to synchronize access between the URL reader coroutine and parallel downloader threads.
- Designed a custom coroutine (ReadUrlsHandle, ReadUrlsPromise) for chunked URL reading with manual suspension/resumption to control data ingestion flow based on downloader readiness.
- Built a libcurl-based downloader using a custom write callback to stream HTTP responses directly into output files without blocking other tasks or threads.
- Spawned parallel download workers using hpx::async, each calling process_downloads to pop URLs and trigger file downloads; worker futures are stored for synchronized completion.
- Python script benchmarks and visualizes metrics like download time, CPU utilization, and latency to evaluate system performance across file sizes and thread counts.