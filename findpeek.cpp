/*
 find peek via binary search
 a is an array whose adjecent elements differs from each other.
 if a[i] > a[i - 1] && a[i] > a[i + 1] 
 then a[i] is a peek
 if a[0] > a[1] then a[0] is peek
 if a[n - 1] > a[n - 2] then a[n - 1] is a peek

 we wanna find the index of the peek via binary search
*/

#include <iostream>
#include <vector>


int findPeek(const std::vector<int> &arr)
{
    int n = arr.size();

    if(n == 0)
        return -1;

    // if the arr has only one elem, then it is peek itself
    if(n < 2) return 0;

    // if the arr has peek at both end.
    if(arr[0] > arr[1])
        return 0;
    if(arr[n - 1] > arr[n - 2])
        return n - 1;
    // if the peek is not at the both end, try to find it in somewhere between.
    int l = 1, r = n - 2, m = 0, ans = -1;

    while(l <= r)
    {
        // check m
        m = l + ((r - l) >> 1);
        // if m is not the peek, besides it goes down
        if(arr[m - 1] > arr[m])
        {
            // then there has to be a peek on the left hand side, given that it goes up between(0,1)
            r = m - 1;
        }
        else if (arr[m] < arr[m + 1]) // if m is not the peek, besides it goes up
        {
            // then there has to ve a peek on the right hand side, given that it goes down between(n - 2,n - 1)
            l = m + 1;
        }
        else
        {
            ans = m;
            break;
        }
    }
    return ans;
}

struct TestCase
{
    std::string name;
    std::vector<int> input;
    // 备注：有些情况可能有多个合法答案，所以我们不硬编码 expectedIndex，而是验证逻辑
};

// 验证器：检查返回的 index 是否真的是一个峰值
bool verifyPeak(const std::vector<int> &arr, int index)
{
    int n = arr.size();
    if (n == 0)
        return index == -1;
    if (n == 1)
        return index == 0;

    if (index < 0 || index >= n)
        return false; // 越界

    // 检查左边界
    if (index == 0)
        return arr[0] > arr[1];
    // 检查右边界
    if (index == n - 1)
        return arr[n - 1] > arr[n - 2];
    // 检查中间
    return (arr[index] >= arr[index - 1]) && (arr[index] >= arr[index + 1]);
    // 注意：原题通常要求严格大于，但你的代码允许 >= (else分支)，这里根据代码行为适配验证逻辑
}

void runTest(const TestCase &test)
{
    std::cout << "[TEST] " << test.name << "... ";

    int resultIndex = findPeek(test.input);

    if (verifyPeak(test.input, resultIndex))
    {
        std::cout << "PASSED. (Index: " << resultIndex;
        if (resultIndex >= 0)
            std::cout << ", Value: " << test.input[resultIndex];
        std::cout << ")" << std::endl;
    }
    else
    {
        std::cout << "FAILED!" << std::endl;
        std::cout << "  Input Size: " << test.input.size() << std::endl;
        std::cout << "  Returned Index: " << resultIndex << std::endl;
        if (resultIndex >= 0 && resultIndex < test.input.size())
            std::cout << "  Value at Index: " << test.input[resultIndex] << std::endl;
        else
            std::cout << "  Index out of bounds!" << std::endl;
    }
}

int main()
{
    std::vector<TestCase> tests = {
        // 1. 极端边界
        {"Empty Array", {}},
        {"Single Element", {10}},
        {"Two Elements Ascending", {1, 10}},
        {"Two Elements Descending", {10, 1}},

        // 2. 基础形态
        {"Simple Pyramid", {1, 2, 3, 2, 1}},
        {"Monotonic Increasing", {1, 2, 3, 4, 5}},
        {"Monotonic Decreasing", {5, 4, 3, 2, 1}},

        // 3. 复杂形态
        {"ZigZag (Multi Peak)", {1, 10, 2, 10, 1}}, // 应该返回 1 或 3
        {"Left Weighted", {1, 10, 5, 4, 3, 2}},     // 峰值在左侧部分
        {"Right Weighted", {1, 2, 3, 4, 10, 1}},    // 峰值在右侧部分

    };

    std::cout << "=== Starting Peak Finding Tests ===" << std::endl;
    for (const auto &t : tests)
    {
        runTest(t);
    }
    std::cout << "=== All Tests Completed ===" << std::endl;

    return 0;
}
