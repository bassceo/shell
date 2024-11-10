#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;

double calculate_ema(const vector<int>& array, double alpha) {
    double ema = array[0];
    for (size_t i = 1; i < array.size(); ++i) {
        ema = alpha * array[i] + (1 - alpha) * ema;
    }
    return ema;
}

vector<int> exponential_moving_average_sort(const vector<int>& arr, double alpha = 0.5) {
    vector<pair<int, double>> ema_with_values;
    for (size_t i = 0; i < arr.size(); ++i) {
        ema_with_values.emplace_back(arr[i], calculate_ema({arr.begin(), arr.begin() + i + 1}, alpha));
    }

    sort(ema_with_values.begin(), ema_with_values.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    vector<int> result;
    for (const auto& p : ema_with_values) {
        result.push_back(p.first);
    }
    return result;
}

int main() {
    vector<int> arr = {10, 20, 5, 40, 25, 15};
    vector<int> sorted_arr = exponential_moving_average_sort(arr);
    cout << "Sorted by EMA: ";
    for (int num : sorted_arr) {
        cout << num << " ";
    }
    cout << endl;
    return 0;
}
