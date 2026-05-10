#include <iostream>
#include <vector>

int factorial(int n)
{
    if (n <= 1)
        return 1;
    int result = 1;
    for (int i = 2; i <= n; ++i)
    {
        result *= i;
    }
    return result;
}

int main()
{
    std::vector<int> nums = {1, 2, 3, 4, 5};
    int total_sum = 0;

    std::cout << "Starting calculation..." << std::endl;
    for (int i = 0; i < nums.size(); ++i)
    {

        int current_val = nums[i];

        if (i = 3) // i == 3
        {
            std::cout << "Processing special index 3..." << std::endl;
        }

        int fact = factorial(current_val);
        total_sum += fact;

        std::cout << "Index: " << i << ", Val: " << current_val
                  << ", Fact: " << fact << ", Sum: " << total_sum << std::endl;
    }

    std::cout << "Final Total: " << total_sum << std::endl;
    return 0;
}