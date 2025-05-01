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

  `./hpx_async_downloader --hpx:threads 4 --benchmarking 0 --batch_size 20 --num_of_urls 100`
- Also generate benchmarks by running `/benchmarks/run_benchmarks.sh` using the following command
```
chmod +x run_benchmarks.sh
./run_benchmarks.sh  
```

### **Sample Output**
```
Time taken: 8.64762 sec
Batch Size: 20
Number of URLs: 100
Total Bytes Downloaded: 109800
Speedup: 3.90333
Number of threads: 4
Efficiency: 97.5833%
Throughput: 12697.1
Latency per task: 7.8758e-05
```

### **Benchmark Results**
- *Strong Parallel Performance*

  HPX async downloader shows near-linear speedup and high efficiency (up to 97.6%) for batch sizes like 20 with 4 threads, completing 100 downloads in just 8.65s.
- *Optimal Batch Size Range*
  
  Batch sizes between 32–256 hit the sweet spot for large number of URLs, balancing task granularity and parallelism without overwhelming the system or underutilizing threads.
- *Thread Count Matters*
  
  Best performance is observed with 2–4 threads. Higher thread counts don’t help much unless the number of URLs is large enough to distribute effectively.
- *Latency & Throughput*
  
  Tasks show low latency (~microseconds) and high throughput (>26,000 bytes/sec), demonstrating efficient task scheduling and minimal I/O bottlenecks.
- *Need for Adaptive Scheduling*
  
  Static configs may under-perform. Dynamically tuning batch size and threads based on workload size offers the best balance of speed, efficiency, and resource use.

### **Implementation Details**
- Implemented `SharedUrlQueue` using `std::mutex`, `std::condition_variable`, and `atomic done_` flag to synchronize access between the URL reader coroutine and parallel downloader threads.
- Designed a custom coroutine (`ReadUrlsHandle`, `ReadUrlsPromise`) for chunked URL reading with manual suspension/resumption to control data ingestion flow based on downloader readiness.
- Built a libcurl-based downloader using a custom write callback to stream HTTP responses directly into output files without blocking other tasks or threads.
- Spawned parallel download workers using `hpx::async`, each calling process_downloads to pop URLs and trigger file downloads; worker futures are stored for synchronized completion.
- Python script benchmarks and visualizes metrics like download time, CPU utilization, and latency to evaluate system performance across file sizes and thread counts.
