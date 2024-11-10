#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>

int binarySearch(const std::vector<int>& arr, int target) {
    int left = 0;
    int right = arr.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }

    return -1;
}

int main() {
    std::vector<int> testArr = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int testTarget = 5;
    int result = binarySearch(testArr, testTarget);
    std::cout << "Index of " << testTarget << " in testArr: " << result << std::endl;

    // Generate a random array and target for testing
    std::vector<int> randomArr(1000);
    for (int i = 0; i < 1000; ++i) {
        randomArr[i] = rand() % 10000; // Random numbers between 0 and 9999
    }
    std::sort(randomArr.begin(), randomArr.end()); // Ensure the array is sorted

    while (true) {
        int randomTarget = rand() % 10000; // Random target between 0 and 9999
        result = binarySearch(randomArr, randomTarget);
        std::cout << "Index of " << randomTarget << " in randomArr: " << result << std::endl;
    }

    return 0;
}
