/*
Binary search has a important premise. that is , the array has to be ordered.
*/

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>

// brutal find
bool right(std::vector<int> arr, int target)
{
    for(const auto num : arr)
    {
        if(num == target)
        {
            return true;
        }
    }
    return false;
}

// binary search
bool exists(std::vector<int> arr, int target)
{
    if(arr.size() == 0) return false;

    int l = 0, r = arr.size() - 1, m = 0;
    while(l <= r)
    {
        m = (l + r) / 2;
        if(arr[m] == target) return true;
        else if (arr[m] > target) r = m - 1;
        else l = m + 1;
    }
    return false;
}

// self-checker
std::vector<int> randomArray(int n, int v)
{
    // random suite
    std::mt19937 rng(std::random_device{}());              // random number generator
    std::uniform_real_distribution<double> dist(0.0, 1.0); // uniform distribution

    // generate given length random array
    std::vector<int> arr = std::vector<int>(n);
    for (int i = 0; i < n; i++)
    {
        arr[i] = (int)(dist(rng) * v) + 1; // [1,v] integer, equal probablility.
    }

    return arr;
}

void printArray(const std::vector<int> &arr)
{
    for (int x : arr)
        std::cout << x << " ";
    std::cout << "\n";
}

int main()
{
    int N = 100;
    int V = 1000;
    int testTimes = 50000;

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    std::cout << "test starts \n";
    for (int i = 0; i < testTimes;i++)
    {
        int n = (int)(dist(rng) * N);
        std::vector<int> arr = randomArray(n,V);
        std::sort(arr.begin(),arr.end());

        int randomNum = (int)(dist(rng) * V);
        // they should return the same is binary search is correct
        if(right(arr,randomNum) != exists(arr,randomNum))
        {
            std::cout << "the [exist] solution is wrong" << "\n";
            printArray(arr);
            return 1;
        }
    }
    std::cout << "test ends \n";
    return 0;
}