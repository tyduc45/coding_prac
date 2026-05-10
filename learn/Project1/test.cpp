// test_flat_hash_map.cpp
#include "flat_hash_table_simd.h"
//#include "flat_hash_table.h"
#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <random>
#include <chrono>

void test_basic() {
    mini_absl::FlatHashMap<int, std::string> m;
    assert(m.empty());
    assert(m.size() == 0);

    auto [p1, ins1] = m.insert_or_assign(1, "one");
    assert(ins1);
    assert(*p1 == "one");
    assert(m.size() == 1);

    auto [p2, ins2] = m.insert_or_assign(1, "ONE");
    assert(!ins2);  // 已存在,应该是更新
    assert(*p2 == "ONE");
    assert(m.size() == 1);

    m.insert_or_assign(2, "two");
    m.insert_or_assign(3, "three");
    assert(m.size() == 3);

    auto* p = m.find(2);
    assert(p && *p == "two");
    assert(m.find(99) == nullptr);

    assert(m.erase(2));
    assert(!m.erase(2));
    assert(m.size() == 2);
    assert(m.find(2) == nullptr);
    assert(m.find(1) != nullptr);
    assert(m.find(3) != nullptr);

    std::cout << "[PASS] test_basic\n";
}

void test_grow() {
    mini_absl::FlatHashMap<int, int> m;
    const int N = 10000;
    for (int i = 0; i < N; ++i) {
        m.insert_or_assign(i, i * 2);
    }
    assert(m.size() == N);
    for (int i = 0; i < N; ++i) {
        auto* p = m.find(i);
        assert(p && *p == i * 2);
    }
    // 删一半
    for (int i = 0; i < N; i += 2) {
        m.erase(i);
    }
    assert(m.size() == N / 2);
    for (int i = 0; i < N; ++i) {
        auto* p = m.find(i);
        if (i % 2 == 0) {
            assert(!p);
        }
        else {
            assert(p && *p == i * 2);
        }
    }
    std::cout << "[PASS] test_grow (N=" << N << ")\n";
}

void test_string_keys() {
    mini_absl::FlatHashMap<std::string, int> m;
    m.insert_or_assign("apple", 1);
    m.insert_or_assign("banana", 2);
    m.insert_or_assign("cherry", 3);
    assert(*m.find("apple") == 1);
    assert(*m.find("banana") == 2);
    assert(*m.find("cherry") == 3);
    assert(m.find("durian") == nullptr);

    int total = 0;
    m.for_each([&](const std::string&, int v) { total += v; });
    assert(total == 6);
    std::cout << "[PASS] test_string_keys\n";
}

void test_against_std() {
    // 与 std::unordered_map 对拍随机操作
    mini_absl::FlatHashMap<int, int> mine;
    std::unordered_map<int, int> ref;

    std::mt19937 rng(42);
    const int OPS = 50000;
    for (int i = 0; i < OPS; ++i) {
        int op = rng() % 3;
        int k = rng() % 1000;
        if (op == 0) {  // insert
            int v = rng();
            mine.insert_or_assign(k, v);
            ref[k] = v;
        }
        else if (op == 1) {  // erase
            bool a = mine.erase(k);
            bool b = ref.erase(k) > 0;
            assert(a == b);
        }
        else {  // find
            int* a = mine.find(k);
            auto it = ref.find(k);
            if (it == ref.end()) {
                assert(a == nullptr);
            }
            else {
                assert(a && *a == it->second);
            }
        }
        assert(mine.size() == ref.size());
    }
    std::cout << "[PASS] test_against_std (ops=" << OPS << ")\n";
}

void bench() {
    const int N = 1'000'000;
    std::mt19937 rng(123);
    std::vector<int> keys(N);
    for (int i = 0; i < N; ++i) keys[i] = rng();

    {
        mini_absl::FlatHashMap<int, int> m;
        auto t0 = std::chrono::steady_clock::now();
        for (int k : keys) m.insert_or_assign(k, k);
        auto t1 = std::chrono::steady_clock::now();
        long long sum = 0;
        for (int k : keys) {
            auto* p = m.find(k);
            if (p) sum += *p;
        }
        auto t2 = std::chrono::steady_clock::now();
        std::cout << "mini_absl insert: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()
            << " ms, lookup: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
            << " ms (sum=" << sum << ")\n";
    }
    {
        std::unordered_map<int, int> m;
        auto t0 = std::chrono::steady_clock::now();
        for (int k : keys) m[k] = k;
        auto t1 = std::chrono::steady_clock::now();
        long long sum = 0;
        for (int k : keys) {
            auto it = m.find(k);
            if (it != m.end()) sum += it->second;
        }
        auto t2 = std::chrono::steady_clock::now();
        std::cout << "std::unordered_map insert: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count()
            << " ms, lookup: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
            << " ms (sum=" << sum << ")\n";
    }
}

int main() {
    test_basic();
    test_grow();
    test_string_keys();
    test_against_std();
    std::cout << "\n--- benchmark ---\n";
    bench();
    return 0;
}