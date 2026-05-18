Set of benchmarks for cpu - to test maximum scaling factors.

Benchmark2 seems to be the best one to measure the performance

-----------------------------------
i4-4770@3.4 GHz
-----------------------------------
Baseline Time: 21.7029s (Pi: 3.14159)

Threads    | Time          | Verification
-----------|---------------|-------------------
         2 |      10.8807s | Result: 3.141592653422
         4 | 5.847461200000s | Result: 3.141592653421
         8 | 5.744139900000s | Result: 3.141592653422
        48 | 5.811910000000s | Result: 3.141592653423

--- Speedup Summary ---
2 Threads:  1.99x speedup
4 Threads:  3.71x speedup
8 Threads:  3.78x speedup
48 Threads: 3.73x speedup

