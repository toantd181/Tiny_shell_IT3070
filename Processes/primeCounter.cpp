#include "primeCounter.h"
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <cmath>
#include <chrono>

using namespace std;
using namespace chrono;

bool PrimeCounter::isPrime(int n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

void PrimeCounter::countRange(int start, int end, atomic<int>& count) {
    for (int i = start; i <= end; ++i) {
        if (isPrime(i)) {
            count++;
        }
    }
}

void PrimeCounter::countPrimes(const vector<string>& args) {
    if (args.size() < 3) {
        cout << "Usage: prime A B num_threads" << endl;
        return;
    }

    // Parse input arguments
    int A = stoi(args[0]);
    int B = stoi(args[1]);
    int numThreads = stoi(args[2]);

    // Validate the range and number of threads
    if (A > B || numThreads <= 0) {
        cout << "Invalid range or thread count." << endl;
        return;
    }

    // Atomic counter to track prime numbers
    atomic<int> primeCount(0);
    vector<thread> threads;

    auto start_time = high_resolution_clock::now();

    // Divide the range into chunks for each thread
    int range = (B - A + 1);
    int chunk = range / numThreads;
    int remainder = range % numThreads;

    int currentStart = A;
    for (int i = 0; i < numThreads; ++i) {
        int currentEnd = currentStart + chunk - 1;
        if (i < remainder) currentEnd++;

        // Create a new thread to process the range of numbers
        threads.emplace_back(countRange, currentStart, currentEnd, ref(primeCount));

        // Update the starting point for the next chunk
        currentStart = currentEnd + 1;
    }

    // Wait for all threads to finish
    for (auto& t : threads) {
        t.join();
    }

    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end_time - start_time);

    // Output the result
    cout << "Total prime numbers between " << A << " and " << B << ": " << primeCount.load() << endl;
    cout << "Time taken with " << numThreads << " threads: "<< duration.count() << " microseconds." << endl;
}
