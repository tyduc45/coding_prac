#include<iostream>
#include<vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

void swap(std::vector<int>& arr, int i, int j)
{
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
}

void selectionSort(std::vector<int>& arr)
{
    if(arr.size() < 2)  return;
    for (int minIndex = 0, i = 0; i < arr.size();i++)
    {
        // initialize
        minIndex = i;
        for (int j = i + 1; j < arr.size();j++)
        {
            // find new minIndex from i+1 ~ end
            if(arr[j] < arr[minIndex])
            {
                minIndex = j;
            }
        }
        swap(arr, i, minIndex);
    }
}

void bubbleSort(std::vector<int>& arr)
{
    if (arr.size() < 2) return;
    for (int end = arr.size() - 1; end > 0;end--)
    {
        for (int i = 0; i < end ; i++)
        {
            if(arr[i] > arr[i+1])
                swap(arr, i, i + 1);
        }
    }
}

void insertSort(std::vector<int>& arr)
{
    if(arr.size() < 2) return;
    for (int start = 1; start < arr.size();start++)
    {
        for (int i = start; i > 0 && arr[i] < arr[i - 1]; i--)
        {
                swap(arr, i, i - 1);
        }
    }
}

bool isSame(const std::vector<int>& a, const std::vector<int>& b)
{
    if (a.size() != b.size())
        return false;
    for (int i = 0; i < a.size(); i++)
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

void runTest(void (*sortFunc)(std::vector<int>&), const std::string name)
{
    std::cout << "\n===== Testing " << name << " =====\n";

    // define test set
    std::vector<std::vector<int>> tests = {
        {},                             // 空数组
        {5},                             // 单元素
        {1, 2, 3, 4, 5},                 // 已排序
        {5, 4, 3, 2, 1},                // 逆序
        {3, 3, 3, 3},                   // 重复
        {9, 1, 5, 3, 7, 2, 8, 6, 4},    // 随机
    };

    srand(time(nullptr));
    for (int i = 0; i < 5;i++)
    {
        std::vector<int> randArr;
        for (int i = 0; i < 10;i++)
            randArr.push_back(rand() % 100);
        tests.push_back(randArr);
    }

    for(auto test : tests)
    {
        std::vector<int> arr = test;
        std::vector<int> stdArr = test;

        // standard answer
        std::sort(stdArr.begin(), stdArr.end());
        //my answer
        sortFunc(arr);

        bool pass = isSame(arr, stdArr);

        std::cout << (pass ? "[PASS] " : "[FAIL] ")
                  << "Input: ";
        printArray(test);

        if (!pass)
        {
            std::cout << "Your output : ";
            printArray(arr);
            std::cout << "Correct sort: ";
            printArray(stdArr);
        }
        std::cout << "----------------------\n";
    }
}

int main()
{
    runTest(selectionSort,"Selection sort");
    runTest(bubbleSort, "bubble Sort");
    runTest(insertSort, "insert Sort");


    return 0;
}