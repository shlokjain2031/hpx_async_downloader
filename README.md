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
- Implemented a generic SharedUrlQueue<T> as a thread-safe queue using std::mutex, std::condition_variable, and an atomic done_ flag. This queue acts as the core communication channel between the producer coroutine (which pushes URLs read from a file) and the downloader workers (which pop URLs to fetch and process download requests in parallel).
- Designed a custom coroutine type ReadUrlsHandle with ReadUrlsPromise to support manual suspension and resumption of the URL reading logic. This coroutine enables chunked processing of input URLs and gives the program full control over when to yield and resume reading, allowing tight coordination with downloader threads.
- Wrote a low-level HTTP downloader using libcurl that fetches data from a given URL and streams it directly into a file using a custom write callback. This function is invoked inside download coroutines to perform efficient I/O without blocking other parts of the pipeline.
- Built a download worker coroutine using HPX’s hpx::async to parallelize file downloads. Each worker thread invokes process_downloads, which pops URLs from the shared queue and triggers libcurl-based fetch requests. The workers are launched in a loop and their futures stored for later synchronization, enabling scalable concurrent downloads across multiple threads.
- Python script to visualise benchmarks like download time, CPU Utilisation and latency and visualise them to using plotting libraries