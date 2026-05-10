
//#include "qsort.h"
#include<iostream>
inline int CountTrailingZeros(uint32_t x) {
#if defined(__GNUC__) || defined(__clang__)
	return __builtin_ctz(x);
#else
	int n = 0;
	while (!(x & 1)) { x >>= 1; ++n; } // 不断向右移动知道找到N
	return n;
#endif
}

std::string ToBinary(int x)
{
	std::string ans = "";
	while (x) { ans += (x % 2) ? "1" : "0"; x /= 2; }
	std::reverse(ans.begin(), ans.end());
	return ans;
}
#ifdef qsort_test
int main()
{
#ifdef sqrt
	std::cout << "The square root of 1000 is approximately: " << sqrt_self(1000) << std::endl;
	std::cout << "The square root of 1000 is approximately: " << Q_sqrt(1000) << std::endl;
	std::cout << "The square root of 1000 is approximately: " << 1000 * Q_InvSqrt(1000) << std::endl;
#endif
	std::cout << "10 in binary: " << ToBinary(128) << std::endl;
	std::cout << "the ans: " << CountTrailingZeros(437) << std::endl;
	return 0;
}
#endif