#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <filesystem>
#include <sstream>
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

    random_device rd;
    mt19937 generator(rd());
    uniform_int_distribution<int> distribution(0, 9999);

    for (int i = 0; i < iterations; i++) {
        for (int j = 0; j < 1000; j++) {
            data[j] = distribution(generator);
        }

        if (program == "bin-search") {
            binarySearch(data, distribution(generator));
        } else if (program == "ema-sort-int") {
            ema_sort_int(data, temp_dir);
        }
    }

    fs::remove_all(temp_dir);
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cerr << "Usage: " << argv[0] << " <program> <num_threads> <iterations>" << endl;
        cerr << "  <program>: bin-search or ema-sort-int" << endl;
        cerr << "  <num_threads>: number of threads" << endl;
        cerr << "  <iterations>: number of iterations per thread" << endl;
        return 1;
    }

    string program = argv[1];
    int num_threads = stoi(argv[2]);
    int iterations = stoi(argv[3]);

    if (program != "bin-search" && program != "ema-sort-int") {
        cerr << "Invalid algorithm. Use 'bin-search' or 'ema-sort-int'." << endl;
        return 1;
    }

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

    cout << "CPU load completed with " << num_threads << " threads and "
         << iterations << " iterations per thread. Execution time: "
         << duration_precise.count() << " seconds." << endl;

    return 0;
}
