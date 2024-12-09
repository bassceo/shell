#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <sstream>
#include <cstdlib>
#include <random>
#include "algorithms/bin-search.h"
#include "algorithms/ema-sort-int.h"

using namespace std;
namespace fs = std::filesystem;

void worker_thread(int iterations, const string& program) {
    vector<int> data(1000);
    stringstream ss;
    ss << "temp_" << this_thread::get_id();
    string temp_dir = ss.str();
    fs::create_directory(temp_dir);
    
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(0, 9999);

    for (int i = 0; i < iterations; i++) {
        // Generate random data
        for (int j = 0; j < 1000; j++) {
            data[j] = dist(rng);
        }
        
        if (program == "bin-search") {
            binarySearch(data, dist(rng));
        } else if (program == "ema-sort-int") {
            ema_sort_int(data, temp_dir);
        }
    }
    
    fs::remove_all(temp_dir);
}

int main() {
    string program;
    int num_threads, iterations;

    cout << "Enter program to execute (bin-search, ema-sort-int): ";
    cin >> program;

    if (program != "bin-search" && program != "ema-sort-int") {
        cout << "Wrong program" << endl;
        return 1;
    }

    cout << "Enter number of threads to use: ";
    cin >> num_threads;

    cout << "Enter iterations per thread: ";
    cin >> iterations;
    
    vector<thread> threads;
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < num_threads; i++) {
        threads.emplace_back(worker_thread, iterations, program);
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end - start);
    auto duration_precise = chrono::duration_cast<chrono::duration<double>>(end - start);
    
    cout << "CPU load executed with " << num_threads << " threads and " 
         << iterations << " iterations per thread. Time taken: " 
         << duration_precise.count() << " seconds." << endl;

    return 0;
}