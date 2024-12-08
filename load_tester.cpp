#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <sstream>
#include <cstdlib>
#include "algorithms/bin-search.hpp"
#include "algorithms/ema-sort-int.hpp"

using namespace std;
namespace fs = std::filesystem;

void worker_thread(int iterations) {
    vector<int> data(1000);
    stringstream ss;
    ss << "temp_" << this_thread::get_id();
    string temp_dir = ss.str();
    fs::create_directory(temp_dir);
    
    for (int i = 0; i < iterations; i++) {
        // Generate random data
        for (int j = 0; j < 1000; j++) {
            data[j] = rand() % 10000;
        }
        
        // Run binary search
        sort(data.begin(), data.end());
        binarySearch(data, rand() % 10000);
        
        // Run external EMA sort
        exponential_moving_average_sort(data, temp_dir);
    }
    
    fs::remove_all(temp_dir);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <num_threads> <iterations>" << endl;
        return 1;
    }

    int num_threads = atoi(argv[1]);
    int iterations = atoi(argv[2]);
    
    vector<thread> threads;
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(worker_thread, iterations);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    cout << "Total execution time: " << duration.count() << "ms" << endl;

    return 0;
}