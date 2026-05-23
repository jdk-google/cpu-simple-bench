#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <time.h>

double getProcessCpuTime() {
    struct timespec ts;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
    return ts.tv_sec + (ts.tv_nsec / 1000000000.0);
}

/**
 * CPU Benchmark - Performance Test (Multi-Threaded Scaling)
 * 
 * This code performs a heavy mathematical computation (Leibniz formula for Pi)
 * to stress the CPU. It measures scaling performance across 4, 8, and 16 threads.
 *
 * Compile with: 
 * g++ -O3 benchmark2.cpp -o benchmark2 -pthread
 */

const long long ITERATIONS = 6000000000LL; 

// The mathematical task: Calculate Pi using Leibniz series
double calculate_pi_partial(long long start, long long end) {
    double sum = 0.0;
    for (long long i = start; i < end; ++i) {
        double term = (i % 2 == 0) ? 1.0 : -1.0;
        sum += term / (2.0 * i + 1.0);
    }
    return sum;
}

// Function to run the benchmark with a specific number of threads
double run_multi_threaded_test(int num_threads) {
    std::vector<std::thread> threads;
    std::vector<double> results(num_threads);
    
    auto start_time = std::chrono::high_resolution_clock::now();
    auto start_cpu_time = getProcessCpuTime();
    
    long long chunk_size = ITERATIONS / num_threads;
    for (int i = 0; i < num_threads; ++i) {
        long long start = i * chunk_size;
        long long end = (i == num_threads - 1) ? ITERATIONS : (i + 1) * chunk_size;
        
        threads.emplace_back([start, end, i, &results]() {
            results[i] = calculate_pi_partial(start, end);
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto end_cpu_time = getProcessCpuTime();

    std::chrono::duration<double> diff = end_time - start_time;
    auto cpu_time_diff = end_cpu_time - start_cpu_time;
    
    // Sum is calculated but not printing every time to keep output clean
    double pi = std::accumulate(results.begin(), results.end(), 0.0) * 4.0;
    
    std::cout << std::setw(10) << num_threads << " | " 
              << std::setprecision(6)
              << std::setw(12) << diff.count() << "s | "
              << std::setw(12) << cpu_time_diff << "s | "
              << "Result: " << std::fixed << std::setprecision(12) << pi << std::endl;
              
    return diff.count();
}

int main() {
    std::cout << "--- CPU Scaling Benchmark ---" << std::endl;
    std::cout << "Hardware Threads Available: " << std::thread::hardware_concurrency() << std::endl;
    std::cout << "Iterations: " << ITERATIONS << "\n" << std::endl;
    
    // 1. Single-threaded Baseline
    std::cout << "Running Baseline (1 Thread)..." << std::endl;
    auto start_cpu_time = getProcessCpuTime();
    auto start_baseline = std::chrono::high_resolution_clock::now();
    double pi_baseline = calculate_pi_partial(0, ITERATIONS) * 4.0;
    auto end_cpu_time = getProcessCpuTime();
    auto end_baseline = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_baseline = end_baseline - start_baseline;
    double t_baseline = diff_baseline.count();
    double baseline_cpu = end_cpu_time - start_cpu_time;
    
    std::cout << "Baseline Time: " << t_baseline << "s. CPU_Time " << baseline_cpu << " s(Pi: " << pi_baseline << ")\n" << std::endl;

    // 2. Comparative Benchmarks
    std::cout << "Threads    | Time          | CPU Time      | Verification" << std::endl;
    std::cout << "-----------|---------------|---------------|-------------------" << std::endl;
    
    double t2 = run_multi_threaded_test(2);
    double t4 = run_multi_threaded_test(4);
    double t8 = run_multi_threaded_test(8);
    double t16 = run_multi_threaded_test(16);
    double t24 = run_multi_threaded_test(24);
    double t32 = run_multi_threaded_test(32);
    double t48 = run_multi_threaded_test(48);

    // 3. Summary Report
    std::cout << "\n--- Speedup Summary ---" << std::fixed << std::setprecision(2) << std::endl;
    std::cout << "2 Threads:  " << t_baseline / t2 << "x speedup" << std::endl;
    std::cout << "4 Threads:  " << t_baseline / t4 << "x speedup" << std::endl;
    std::cout << "8 Threads:  " << t_baseline / t8 << "x speedup" << std::endl;
    std::cout << "16 Threads: " << t_baseline / t16 << "x speedup" << std::endl;
    std::cout << "24 Threads: " << t_baseline / t24 << "x speedup" << std::endl;
    std::cout << "32 Threads: " << t_baseline / t32 << "x speedup" << std::endl;
    std::cout << "48 Threads: " << t_baseline / t48 << "x speedup" << std::endl;
    
    std::cout << "\nNote: Speedup > Hardware Threads indicates Hyper-threading or Turbo Boost influence." << std::endl;

    return 0;
}
