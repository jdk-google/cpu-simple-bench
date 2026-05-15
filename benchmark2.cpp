#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <numeric>
#include <iomanip>

/**
 * CPU Benchmark - Performance Test (Multi-Threaded Scaling)
 * 
 * This code performs a heavy mathematical computation (Leibniz formula for Pi)
 * to stress the CPU. It measures scaling performance across 4, 8, and 16 threads.
 */

const long long ITERATIONS = 1000000000LL; 

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
    std::chrono::duration<double> diff = end_time - start_time;
    
    // Sum is calculated but not printing every time to keep output clean
    double pi = std::accumulate(results.begin(), results.end(), 0.0) * 4.0;
    
    std::cout << std::setw(10) << num_threads << " | " 
              << std::setw(12) << diff.count() << "s | "
              << "Result: " << std::fixed << std::setprecision(8) << pi << std::endl;
              
    return diff.count();
}

int main() {
    std::cout << "--- CPU Scaling Benchmark ---" << std::endl;
    std::cout << "Hardware Threads Available: " << std::thread::hardware_concurrency() << std::endl;
    std::cout << "Iterations: " << ITERATIONS << "\n" << std::endl;
    
    // 1. Single-threaded Baseline
    std::cout << "Running Baseline (1 Thread)..." << std::endl;
    auto start_baseline = std::chrono::high_resolution_clock::now();
    double pi_baseline = calculate_pi_partial(0, ITERATIONS) * 4.0;
    auto end_baseline = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_baseline = end_baseline - start_baseline;
    double t_baseline = diff_baseline.count();
    
    std::cout << "Baseline Time: " << t_baseline << "s (Pi: " << pi_baseline << ")\n" << std::endl;

    // 2. Comparative Benchmarks
    std::cout << "Threads    | Time          | Verification" << std::endl;
    std::cout << "-----------|---------------|-------------------" << std::endl;
    
    double t4 = run_multi_threaded_test(4);
    double t8 = run_multi_threaded_test(8);
    double t16 = run_multi_threaded_test(16);

    // 3. Summary Report
    std::cout << "\n--- Speedup Summary ---" << std::endl;
    std::cout << "4 Threads:  " << std::fixed << std::setprecision(2) << t_baseline / t4 << "x speedup" << std::endl;
    std::cout << "8 Threads:  " << t_baseline / t8 << "x speedup" << std::endl;
    std::cout << "16 Threads: " << t_baseline / t16 << "x speedup" << std::endl;
    
    std::cout << "\nNote: Speedup > Hardware Threads indicates Hyper-threading or Turbo Boost influence." << std::endl;

    return 0;
}
