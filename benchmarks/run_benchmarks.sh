  #!/bin/bash

  # Output CSV header
  echo "batch_size,num_of_urls,total_parallel_bytes_downloaded,num_threads,time_taken,speedup,efficiency,throughput_parallel,latency_per_task_parallel,throughput_sequential,latency_per_task_sequential" > benchmark_results.csv

  # Define batch sizes and thread counts
  batch_size=(32 128 512 1024 2048 4096 8192)
  thread_counts=(2 4 8)

  # Loop over batch sizes and thread counts
  for size in "${batch_size[@]}"; do
    for threads in "${thread_counts[@]}"; do
      # Run your executable, capture output
      result=$(/Users/shlokjain/CLionProjects/hpx_async_downloader/cmake-build-release/hpx_async_downloader --hpx:threads $threads --benchmarking 1 --batch_size $size --num_of_urls 200000)
      # Extract the CSV line from program output
      csv_line=$(echo "$result" | grep -E '^[0-9]+,')  # Assumes your program prints only one CSV line per run

      # Append to CSV file
      echo "$csv_line" >> benchmark_results.csv

      echo "Completed: Batch Size $size, Threads $threads"
    done
  done

  echo "Benchmarking completed! Results saved to benchmark_results.csv"

# # Run the Python script to plot the results
# echo "Generating plots..."
# /Users/shlokjain/CLionProjects/hpx_async_downloader/.venv/bin/python /Users/shlokjain/CLionProjects/hpx_async_downloader/benchmarks/benchmark_plotter.py
#
# echo "Plots generated and saved!"