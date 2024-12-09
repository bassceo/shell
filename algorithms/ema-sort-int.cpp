#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <queue>

using namespace std;

// Write chunk to temp file with calculated EMAs
void write_chunk(const vector<int>& chunk, const string& filename) {
    ofstream out(filename, ios::binary);
    int size = chunk.size();
    out.write(reinterpret_cast<const char*>(&size), sizeof(size));
    out.write(reinterpret_cast<const char*>(chunk.data()), size * sizeof(int));
}

// Read chunk from temp file
vector<int> read_chunk(const string& filename) {
    ifstream in(filename, ios::binary);
    int size;
    in.read(reinterpret_cast<char*>(&size), sizeof(size));
    vector<int> chunk(size);
    in.read(reinterpret_cast<char*>(chunk.data()), size * sizeof(int));
    return chunk;
}

// Merge multiple sorted chunks
void merge_files(const vector<string>& chunk_files, const string& output_file) {
    vector<ifstream> inputs;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> heap;

    // Open all input files and initialize heap
    for (size_t i = 0; i < chunk_files.size(); i++) {
        inputs.emplace_back(chunk_files[i], ios::binary);
        int value;
        if (inputs[i].read(reinterpret_cast<char*>(&value), sizeof(int))) {
            heap.push({value, i});
        }
    }

    ofstream out(output_file, ios::binary);
    
    // Merge chunks using min-heap
    while (!heap.empty()) {
        auto [value, index] = heap.top();
        heap.pop();
        
        out.write(reinterpret_cast<const char*>(&value), sizeof(int));
        
        int next_value;
        if (inputs[index].read(reinterpret_cast<char*>(&next_value), sizeof(int))) {
            heap.push({next_value, index});
        }
    }
}

vector<int> ema_sort_int(const vector<int>& arr, const string& temp_dir) {
    const size_t CHUNK_SIZE = 1024 * 1024; // 1MB chunks
    vector<string> chunk_files;
    
    // Create temp directory if it doesn't exist
    filesystem::create_directories(temp_dir);

    // Split into chunks and sort each chunk
    for (size_t i = 0; i < arr.size(); i += CHUNK_SIZE) {
        vector<int> chunk;
        for (size_t j = i; j < min(i + CHUNK_SIZE, arr.size()); j++) {
            chunk.push_back(arr[j]);
        }
        
        // Calculate EMA for chunk elements
        double ema = chunk[0];
        for (size_t j = 1; j < chunk.size(); j++) {
            ema = 0.5 * chunk[j] + 0.5 * ema;
        }
        
        sort(chunk.begin(), chunk.end());
        
        string chunk_file = temp_dir + "/chunk_" + to_string(i/CHUNK_SIZE) + ".tmp";
        write_chunk(chunk, chunk_file);
        chunk_files.push_back(chunk_file);
    }

    // Merge sorted chunks
    string output_file = temp_dir + "/sorted.tmp";
    merge_files(chunk_files, output_file);

    // Read final sorted result
    vector<int> result = read_chunk(output_file);

    // Cleanup temporary files
    for (const auto& file : chunk_files) {
        filesystem::remove(file);
    }
    filesystem::remove(output_file);

    return result;
}