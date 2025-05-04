#ifndef PRIMECOUNTER_H
#define PRIMECOUNTER_H

#include <vector>
#include <string>
#include <atomic>

class PrimeCounter {
public:
    static void countPrimes(const std::vector<std::string>& args);
private:
    static bool isPrime(int n);
    static void countRange(int start, int end, std::atomic<int>& count);
};

#endif // PRIMECOUNTER_H
