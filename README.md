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

-----------------
AMD EPYC 7B13 - 64 Core VM
-----------------
--- CPU Scaling Benchmark ---
Hardware Threads Available: 64
Baseline Time: 8.394s (Pi: 3.14159)

Threads    | Time          | Verification
-----------|---------------|-------------------
         2 |      4.19935s | Result: 3.141592653422
         4 | 2.100325172000s | Result: 3.141592653421
         8 | 1.050996614000s | Result: 3.141592653422
        16 | 0.526232443000s | Result: 3.141592653422
        24 | 0.352495680000s | Result: 3.141592653423
        32 | 0.264539397000s | Result: 3.141592653422
        48 | 0.330661683000s | Result: 3.141592653423

--- Speedup Summary ---
2 Threads:  2.00x speedup
4 Threads:  4.00x speedup
8 Threads:  7.99x speedup
16 Threads: 15.95x speedup
24 Threads: 23.81x speedup
32 Threads: 31.73x speedup
48 Threads: 25.39x speedup

