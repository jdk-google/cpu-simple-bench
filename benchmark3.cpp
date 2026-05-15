#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <numeric>
#include <iomanip>
#include <algorithm>

/**
 * CPU Benchmark - High Intensity Scaling Analysis
 * 
 * DESIGN NOTES:
 * 1. Arithmetic Intensity: We use a more complex math kernel (Sin/Cos/Sqrt) 
 *    to ensure the CPU execution units are fully occupied and to prevent 
 *    simple compiler optimizations from trivializing the workload.
 * 2. Cache Alignment: We use local accumulation to prevent false sharing.
 * 3. Scaling: We measure speedup across 1, 4, 8, and 16 threads.
 */

// Total workload size
const long long ITERATIONS = 100000000LL; 

// The mathematical "Kernel" - intentionally complex to stress the ALU/FPU
// This prevents the compiler from converting the loop into a simple SIMD broadside
double heavy_math_kernel(long long i) {
    double x = static_cast<double>(i) + 1.0;
    // Chain of transcendental functions to keep pipelines busy
    double val = std::sin(x) * std::cos(x);
    val = std::sqrt(std::abs(val)) + std::pow(x, -0.5);
    return (i % 2 == 0 ? 1.0 : -1.0) * val / (2.0 * x + 1.0);
}

// Partial calculation function
double calculate_partial(long long start, long long end) {
    double local_sum = 0.0;
    for (long long i = start; i < end; ++i) {
        local_sum += heavy_math_kernel(i);
    }
    return local_sum;
}

struct TestResult {
    int threads;
    double seconds;
    double speedup;
    double efficiency;
};

TestResult run_test(int num_threads, double baseline_time) {
    std::vector<std::thread> threads;
    std::vector<double> results(num_threads, 0.0);
    
    auto start = std::chrono::high_resolution_clock::now();
    
    long long chunk = ITERATIONS / num_threads;
    for (int i = 0; i < num_threads; ++i) {
        long long s = i * chunk;
        long long e = (i == num_threads - 1) ? ITERATIONS : (i + 1) * chunk;
        
        threads.emplace_back([s, e, i, &results]() {
            results[i] = calculate_partial(s, e);
        });
    }

    for (auto& t : threads) t.join();
    
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    
    double t_sec = diff.count();
    double speedup = (baseline_time > 0) ? (baseline_time / t_sec) : 1.0;
    double efficiency = (speedup / num_threads) * 100.0;

    return { num_threads, t_sec, speedup, efficiency };
}

int main() {
    std::cout << "===========================================" << std::endl;
    std::cout << "   CPU SCALING & STRESS TEST BENCHMARK     " << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "Hardware Concurrency: " << std::thread::hardware_concurrency() << " logical cores" << std::endl;
    std::cout << "Workload Intensity:   High (Transcendental Math)" << std::endl;
    std::cout << "Iterations:           " << ITERATIONS << std::endl;
    std::cout << "-------------------------------------------" << std::endl;

    // 1. Establish Baseline
    std::cout << "[*] Establishing Baseline (1 Thread)... " << std::flush;
    auto start_base = std::chrono::high_resolution_clock::now();
    calculate_partial(0, ITERATIONS);
    auto end_base = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff_base = end_base - start_base;
    double baseline = diff_base.count();
    std::cout << std::fixed << std::setprecision(3) << baseline << "s" << std::endl;

    // 2. Run Scalability Suite
    std::vector<int> thread_counts = {4, 8, 16};
    std::vector<TestResult> reports;
    reports.push_back({1, baseline, 1.0, 100.0});

    std::cout << "\n[*] Running Multithreaded Suite..." << std::endl;
    std::cout << std::setw(8) << "Threads" << " | " 
              << std::setw(10) << "Time (s)" << " | " 
              << std::setw(8) << "Speedup" << " | " 
              << std::setw(10) << "Efficiency" << std::endl;
    std::cout << "---------|------------|----------|-----------" << std::endl;

    for (int count : thread_counts) {
        TestResult res = run_test(count, baseline);
        reports.push_back(res);
        std::cout << std::setw(8) << res.threads << " | " 
                  << std::setw(10) << res.seconds << " | " 
                  << std::setw(7) << res.speedup << "x | " 
                  << std::setw(9) << res.efficiency << "%" << std::endl;
    }

    // 3. Expert Analysis
    std::cout << "\n[!] SCALING ANALYSIS:" << std::endl;
    double max_speedup = 0;
    for(auto& r : reports) if(r.speedup > max_speedup) max_speedup = r.speedup;

    if (max_speedup < (std::thread::hardware_concurrency() * 0.5)) {
        std::cout << "CAUTION: Sub-linear scaling detected (<50% efficiency)." << std::endl;
        std::cout << "Possible causes:" << std::endl;
        std::cout << "- Thermal Throttling: High load causing CPU to downclock." << std::endl;
        std::cout << "- Memory Bandwidth: Kernels hitting cache-miss latency." << std::endl;
        std::cout << "- SMT/Hyper-threading: Logical cores sharing physical units." << std::endl;
        std::cout << "- OS Overhead: High context-switching penalty." << std::endl;
    } else {
        std::cout << "SUCCESS: Scaling is within optimal parameters for this architecture." << std::endl;
    }

    std::cout << "\nCompile hint: g++ -O3 -march=native benchmark.cpp -o benchmark -pthread" << std::endl;
    std::cout << "===========================================" << std::endl;

    return 0;
}
