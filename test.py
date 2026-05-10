import math

def solve_user_strategy_v1(hps):
    """解法 A: 原始两步走逻辑"""
    n = len(hps)
    K = [max(0, hp - n) for hp in hps]
    sum_K = sum(K)
    
    T_primes = []
    for i in range(n):
        threshold = hps[i] - math.floor(hps[i] / 2)
        T_primes.append(max(0, threshold - K[i]))
    
    T_primes.sort()
    extra = 0
    for j in range(n):
        if T_primes[j] > j:
            diff = T_primes[j] - j
            extra += diff
            # 注意：这里的补齐是为了触发，j 会随着循环自增
    return sum_K + extra

def solve_user_strategy_v2(hps):
    n = len(hps)
    pre_attacks = [max(0,hps[i] - n) for i in range(n)]
    aoe_threshold = [hps[i] - hps[i] // 2 for i in range(n)]
    remain = 0
    rest = [hps[i] - pre_attacks[i] for i in range(n)]
    rest_rest = []
    for j in range(n):
        temp = rest[j] - aoe_threshold[j]
        if temp <= 0:
            temp = 0
        rest_rest.append(temp)
    rest_rest.sort()

    for k,gap in enumerate(rest_rest):
        if gap > k:
            remain += (gap - k)
    return sum(pre_attacks) + remain

# 测试函数
test_cases = [
    [2, 4],          # n=2, A:[2,4], T:[1,2]. Pre:[0,2]. T':[1,0]. Sorted T':[0,1]. Res: 2+0=2
    [1, 10],         # n=2, A:[1,10], T:[1,5]. Pre:[0,8]. T':[1,0]. Sorted T':[0,1]. Res: 8+0=8
    [7, 13, 2],      # n=3, A:[7,13,2], T:[4,7,1]. Pre:[4,10,0]. T':[0,0,1]. Sorted T':[0,0,1]. Res: 14+0=14
    [100, 100, 100], # n=3, A:[100,100,100], T:[50,50,50]. Pre:[97,97,97]. T':[0,0,0]. Res: 291
    [5, 5, 5],       # n=3, A:[5,5,5], T:[3,3,3]. Pre:[2,2,2]. T':[1,1,1]. Sorted T':[1,1,1]. Res: 6+1=7
    [1,3,6]
]

print(f"{'输入 HPs':<20} | {'解法 A (两步走)':<15} | {'解法 B (削减到n)':<15}")
print("-" * 55)
for tc in test_cases:
    a = solve_user_strategy_v1(tc)
    b = solve_user_strategy_v2(tc)
    print(f"{str(tc):<20} | {a:<15} | {b:<15}")