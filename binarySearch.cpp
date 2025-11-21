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

int rightLeft(std::vector<int> arr, int target)
{
    for (int i = 0; i < arr.size();  i++)
    {
        if(arr[i] >= target)
        {
            return i;
        }
    }
    return -1;
}

/*
 binary search always abort elements that is not needed, so m always stands for the boundary of needed elements
*/
int find_big_left(std::vector<int> arr, int target)
{
    if (arr.size() == 0)
        return -1;

    int l = 0, r = arr.size() - 1, m = 0, ans = -1;
    while (l <= r)
    {
        m = (l + r) / 2;
        if(arr[m] >= target)
        {
            ans = m;
            r = m - 1;
        }
        else
        {
            l = m + 1;
        }
    }
    return ans;
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

template<typename reType>
void runTest(reType (*binaFunc)(std::vector<int>, int), reType (*binaFunc1)(std::vector<int>, int),
             int N, int V, int testTimes,
             std::mt19937 rng,
             std::uniform_real_distribution<double> dist)
{
    for (int i = 0; i < testTimes; i++)
    {
        int n = (int)(dist(rng) * N);
        std::vector<int> arr = randomArray(n, V);
        std::sort(arr.begin(), arr.end());

        int randomNum = (int)(dist(rng) * V);
        // they should return the same is binary search is correct
        if (binaFunc(arr, randomNum) != binaFunc1(arr, randomNum))
        {
            std::cout << "the [exist] solution is wrong" << "\n";
            printArray(arr);
            return;
        }
    }
}

int main()
{
    int N = 100;
    int V = 1000;
    int testTimes = 50000;

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> dist(0.0, 1.0);

    std::cout << "test starts \n";
    runTest<bool>(right,exists,N,V,testTimes,rng,dist);
    runTest<int>(rightLeft, find_big_left, N, V, testTimes, rng, dist);
    std::cout << "test ends \n";

    return 0;
}