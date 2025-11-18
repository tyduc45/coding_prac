/*
 ensure your brutal algorithm pass the test cases.
 and when you also has a optimized solution
 you can use random number generator to generate multiple test cases
 implement brutal first, and implement your optimized solution, compare the answers.
 print the test cases that fails
 https://www.bilibili.com/video/BV1mM4y1H7fz/?spm_id_from=333.1387.homepage.video_card.click&vd_source=34799eaecd5746297a862954c669cc6a
*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <random>


void swap(std::vector<int> &arr, int i, int j)
{
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

void selectionSort(std::vector<int> &arr)
{
    if ((int)arr.size() < 2)
        return;
    for (int minIndex = 0, i = 0; i < (int)arr.size(); i++)
    {
        // initialize
        minIndex = i;
        for (int j = i + 1; j < (int)arr.size(); j++)
        {
            // find new minIndex from i+1 ~ end
            if (arr[j] < arr[minIndex])
            {
                minIndex = j;
            }
        }
        swap(arr, i, minIndex);
    }
}

void bubbleSort(std::vector<int> &arr)
{
    if ((int)arr.size() < 2)
        return;
    for (int end = (int)arr.size() - 1; end > 0; end--)
    {
        for (int i = 0; i < end; i++)
        {
            if (arr[i] > arr[i + 1])
                swap(arr, i, i + 1);
        }
    }
}

void insertSort(std::vector<int> &arr)
{
    if ((int)arr.size() < 2)
        return;
    for (int start = 1; start < (int)arr.size(); start++)
    {
        for (int i = start; i > 0 && arr[i] < arr[i - 1]; i--)
        {
            swap(arr, i, i - 1);
        }
    }
}

// self-checker
std::vector<int> randomArray(int n, int v)
{
    // random suite
    std::mt19937 rng(std::random_device{}()); // random number generator
    std::uniform_real_distribution<double> dist(0.0, 1.0); // uniform distribution

    //generate given length random array
    std::vector<int> arr = std::vector<int>(n);
    for (int i = 0; i < n;i++)
    {
        arr[i] = (int) (dist(rng) * v) + 1; // [1,v] integer, equal probablility.
    }

    return arr;
}

// copy array
std::vector<int> copyArray(std::vector<int> arr)
{
    return arr;
}

bool isSame(const std::vector<int> &a, const std::vector<int> &b)
{
    if (a.size() != b.size())
        return false;
    for (int i = 0; i < (int)a.size(); i++)
        if (a[i] != b[i])
            return false;
    return true;
}

void printArray(const std::vector<int> &arr)
{
    for (int x : arr)
        std::cout << x << " ";
    std::cout << "\n";
}

int main()
{
    int N = 10;
    int V = 5;
    int testTimes = 50000;

    std::cout << "test start\n";

    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<double> uniform_dist(0.0,1.0);

    for (int i = 0; i < testTimes;i++)
    { 
        int n = (int)(uniform_dist(rng) * N);   // get random length
        std::vector<int> arr = randomArray(n, V);

        std::vector<int> arr1 = copyArray(arr); // backup
        std::vector<int> arr2 = copyArray(arr); // backup
        std::vector<int> arr3 = copyArray(arr); // backup
        

        selectionSort(arr1);
        bubbleSort(arr2);
        insertSort(arr3);

        if (!isSame(arr1, arr2) || !isSame(arr1, arr3))
        {
            printf("something went wrong");
            printArray(arr);
            break;
        }
    }
    std::cout << "test end\n";
    return 0;
}
