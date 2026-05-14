#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <numeric>
#include <iomanip>

/**
 * CPU Benchmark - Performance Test
 * 
 * This code performs a heavy mathematical computation (Leibniz formula for Pi)
 * to stress the CPU. It measures both single-threaded and multithreaded performance.
 */

// Number of iterations for the math task
// High enough to take a few seconds
const long long ITERATIONS = 2500000000LL; 

// The mathematical task: Calculate Pi using Leibniz series
// Pi/4 = 1 - 1/3 + 1/5 - 1/7 + ...
double calculate_pi_partial(long long start, long long end) {
    double sum = 0.0;
    for (long long i = start; i < end; ++i) {
        double term = (i % 2 == 0) ? 1.0 : -1.0;
        sum += term / (2.0 * i + 1.0);
    }
    return sum;
}

int main() {
    std::cout << "--- CPU Performance Benchmark ---" << std::endl;
    
    // 1. Single-threaded Benchmark
    std::cout << "\n[1/2] Running single-threaded test..." << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    double pi_single = calculate_pi_partial(0, ITERATIONS) * 4.0;
    
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_single = end_time - start_time;
    
    std::cout << "Single-threaded result: " << std::fixed << std::setprecision(10) << pi_single << std::endl;
    std::cout << "Time taken: " << diff_single.count() << " seconds" << std::endl;

    // 2. Multithreaded Benchmark
    unsigned int num_threads = std::thread::hardware_concurrency();
    if (num_threads == 0) num_threads = 2; // Fallback

    std::cout << "\n[2/2] Running multithreaded test (" << num_threads << " threads)..." << std::endl;
    std::vector<std::thread> threads;
    std::vector<double> results(num_threads);
    
    start_time = std::chrono::high_resolution_clock::now();
    
    long long chunk_size = ITERATIONS / num_threads;
    for (unsigned int i = 0; i < num_threads; ++i) {
        long long start = i * chunk_size;
        long long end = (i == num_threads - 1) ? ITERATIONS : (i + 1) * chunk_size;
        
        threads.emplace_back([start, end, i, &results]() {
            results[i] = calculate_pi_partial(start, end);
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    double pi_multi = std::accumulate(results.begin(), results.end(), 0.0) * 4.0;
    
    end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_multi = end_time - start_time;

    std::cout << "Multithreaded result: " << pi_multi << std::endl;
    std::cout << "Time taken: " << diff_multi.count() << " seconds" << std::endl;
    
    // Summary
    std::cout << "\n--- Summary ---" << std::endl;
    std::cout << "Speedup: " << diff_single.count() / diff_multi.count() << "x" << std::endl;
    std::cout << "Efficiency: " << (diff_single.count() / diff_multi.count() / num_threads) * 100.0 << "%" << std::endl;

    return 0;
}
