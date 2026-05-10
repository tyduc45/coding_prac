#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <utility>
#include <vector>
#include <stdexcept>
#include<new>

namespace mini_absl {
	//control byte
	using ctrl_t = int8_t;

	constexpr ctrl_t kEmpty = static_cast<ctrl_t>(0b10000000); // -128
	constexpr ctrl_t kDeleted = static_cast<ctrl_t>(0b11111110); // -2
	constexpr ctrl_t kSentinel = static_cast<ctrl_t>(0b11111111); // -1

	inline bool IsEmpty(ctrl_t ctrl) {	return ctrl == kEmpty; }
	inline bool IsDeleted(ctrl_t ctrl) { return ctrl == kDeleted; }
	inline bool IsFull(ctrl_t ctrl) { return ctrl >= 0; }
	inline bool IsEmtpyOrDeleted(ctrl_t ctrl) { return ctrl < kSentinel; }

	// H1 = 高 57 位,用于定位"组" (group)
	// H2 = 低 7 位,用于在组内快速过滤
	inline size_t H1(size_t hash) { return hash >> 7; }
	inline size_t H2(size_t hash) { return hash & 0x7F; } // 0b01111111,取低7位

	// 一次性处理16个元素
	constexpr size_t kGroupSize = 16;

	class Group {
	 public:
			explicit Group(const ctrl_t* pos)
			{
				std::memcpy(ctrl_, pos, kGroupSize);
			}
			// 返回一个位掩码: 第 i 位为 1 表示组内第 i 个槽的 H2 匹配
			uint32_t Match(ctrl_t h2) const
			{
				uint32_t mask = 0;
				for (int i = 0; i < kGroupSize; i++){
					if (ctrl_[i] == h2) mask |= (1u << i);
				}
				return mask;
			}
			// 返回一个位掩码: 第 i 位为 1 表示该槽为空
			uint32_t MatchEmpty() const
			{
				uint32_t mask = 0;
				for (int i = 0; i < kGroupSize; i++) {
					if (IsEmpty(ctrl_[i])) mask |= (1u << i);
				}
				return mask;
			}
			// 返回一个位掩码: 第 i 位为 1 表示该槽为空或已删除(可用于插入)
			uint32_t MatchEmptyOrDeleted() const {
				uint32_t mask = 0;
				for (int i = 0; i < kGroupSize; i++) {
					if (IsEmtpyOrDeleted(ctrl_[i])) mask |= (1u << i);
				}
				return mask;
			}

	 private:
		ctrl_t ctrl_[kGroupSize];
	};
	// 找到 mask 中最低的 1 位的位置 (count trailing zeros)
	inline int CountTrailingZeros(uint32_t x) {
#if defined(__GNUC__) || defined(__clang__)
		return __builtin_ctz(x);
#else
		int n = 0; 
		while (!(x & 1)) { x >>= 1; ++n; } // 不断向右移动知道找到N
		return n;
#endif
	}
	// 检测表中空位的辅助类
	class ProbeSeq {
	public:
		ProbeSeq(size_t hash, size_t mask)
			: mask_(mask), offset_(hash & mask), index_(0) {}
		size_t offset() const { return offset_; }
		size_t offset(size_t i) const { return (offset_ + i) & mask_; } 

		// 二次探测设计和线性探测缺陷
		/*
		* // 假设kgroupsize为4 ， 那么idnex = 4，8，12。。
		* 也就是每向下探测一次， 都会一次探测1组，2组，3组，，，
		* 如果是线性探测即offset_ = (offset_ + kgroupsize) & mask_;
		* 相邻元素会堆积到一块连续区域，再开放寻址法中， 导致新插入的元素必须长距离越过他们才能找到空位
		* 这就是主聚集问题
		* 二次探测能够将数据打散，缓解主聚集瓶颈
		*/
		void next() {
			index_ += kGroupSize;  
			offset_ = (offset_ + index_) & mask_;
		}

	private:
		size_t mask_;
		size_t offset_;
		size_t index_;
	};

	// ============ 容量必须是 2 的幂 ============
	inline size_t NormalizeCapacity(size_t n) {
		if (n < kGroupSize) return kGroupSize;
		// 向上取到 2 的幂
		size_t cap = 1;
		while (cap < n) cap <<= 1;
		return cap;
	}

	//注意，faltmap不能满，一旦满就肯定死循环， 所以要设计负载因子并定期扩容
	template<typename K, typename V,
			 typename Hash = std::hash<K>,
			 typename KeyEqual = std::equal_to<K>>
	class FlatHashMap {
	  public:
		using key_type = K;
		using mapped_type = V;
		using value_type = std::pair<K, V>;

		FlatHashMap() : ctrl_(nullptr), slots_(nullptr),
			size_(0), capacity_(0),
			growth_left_(0) {}

		~FlatHashMap() { destroy_all(); }

		FlatHashMap(const FlatHashMap&) = delete;
		FlatHashMap& operator=(const FlatHashMap&) = delete;

		size_t size() const { return size_; }
		size_t capacity() const { return capacity_; }
		bool empty() const { return size_ == 0; }

		V* find(const K& key)
		{
			if (capacity_ == 0) return nullptr;
			size_t hash = Hash{}(key);
			ProbeSeq seq(H1(hash), capacity_ - 1);
			ctrl_t h2 = H2(hash);

			while (true) {
				Group g(ctrl_ + seq.offset());  

				uint32_t match = g.Match(h2); 
				while (match != 0) {
					int i = CountTrailingZeros(match);  
					size_t idx = seq.offset(i);
					if (KeyEqual{}(slots_[idx].first, key)) {
						return &slots_[idx].second;
					}
					match &= match - 1;
				}
				// 只要有empty就说明探测连断掉，那么key不存在
				// 假设人一旦出生就会死，且死的时候一定有尸体，那么如果连尸体都没有，那么这个人一定没出生
				if (g.MatchEmpty() != 0) return nullptr;
				seq.next();
			}

		}

		std::pair<V*, bool> insert_or_assign(const K& key, V value)
		{
			if (capacity_ == 0 || growth_left_ == 0) {
				rehash_and_grow();
			}

			size_t hash = Hash{}(key);

			// find if key exists
			{
				ProbeSeq seq(H1(hash), capacity_ - 1);
				ctrl_t h2 = H2(hash);

				while (true){
					Group g(ctrl_ + seq.offset());
					uint32_t match = g.Match(h2);
					while (match != 0)
					{
						int i = CountTrailingZeros(match);
						size_t idx = seq.offset(i);
						if (KeyEqual{}(slots_[idx].first, key))
						{
							slots_[idx].second = value;
							return { &slots_[idx].second , false };
						}
						match &= match - 1;
					}
					if (g.MatchEmpty() != 0) break;
					seq.next();
				}
			}
			// 找到第一个可插入位置（empty或者deleted）
			size_t target = find_first_not_full(hash);
			set_ctrl(target, H2(hash));
			new (&slots_[target]) value_type(key, std::move(value));
			++size_;
			--growth_left_;
			return { &slots_[target].second, true };
		}

		bool erase(const K& key)
		{
			size_t hash = Hash{}(key);
			ProbeSeq seq(H1(hash), capacity_ - 1);
			ctrl_t h2 = H2(hash);
			while (true)
			{
				Group g(ctrl_ + seq.offset());
				size_t match = g.Match(h2);
				while (match != 0) {
					int i = CountTrailingZeros(match);
					size_t idx = seq.offset(i);
					if (KeyEqual{}(slots_[idx].first, key)) {
						erase_at(idx);
						return true;
					}
					match &= match - 1;
				}
				if (g.MatchEmpty() != 0) {
					return false;
				}
				seq.next();
			}
		}

		template <typename F>
		void for_each(F f) const {
			for (size_t i = 0; i < capacity_; ++i) {
				if (IsFull(ctrl_[i])) {
					f(slots_[i].first, slots_[i].second);
				}
			}
		}
	private:
		ctrl_t* ctrl_;
		value_type* slots_;
		size_t size_;
		size_t capacity_;
		size_t growth_left_;

		void set_ctrl(size_t i, ctrl_t c) {
			ctrl_[i] = c;
			if (i < kGroupSize){
				ctrl_[capacity_ + i] = c;
			}
		}
		size_t find_first_not_full(size_t hash)
		{
			ProbeSeq seq(H1(hash), capacity_ - 1);
			ctrl_t h2 = H2(hash);
			while (true)
			{
				Group g(ctrl_ + seq.offset());
				uint32_t mask = g.MatchEmptyOrDeleted();
				if (mask != 0)	{
					return seq.offset(CountTrailingZeros(mask));
				}
				seq.next();
			}
		}

		void erase_at(size_t idx) {
			std::destroy_at(slots_ + idx);
			set_ctrl(idx, kDeleted);
			--size_;
		}

		static size_t max_load_size(size_t cap) {
			return cap - cap / 8;
		}

		void rehash_and_grow() {
			size_t new_cap = capacity_ == 0 ? kGroupSize : capacity_ * 2;
			rehash(new_cap);
		}

		void rehash(size_t new_cap)
		{
			new_cap = NormalizeCapacity(new_cap);

			ctrl_t* old_ctrl_ = ctrl_;
			value_type* old_slots_ = slots_;
			size_t old_cap = capacity_;

			ctrl_ = static_cast<ctrl_t*>(::operator new(sizeof(ctrl_t) * new_cap + kGroupSize));
			slots_ = static_cast<value_type*>(::operator new(sizeof(value_type) * new_cap));
			memset(ctrl_, kEmpty, new_cap + kGroupSize);
			capacity_ = new_cap;
			size_ = 0;
			growth_left_ = max_load_size(capacity_);

			if (old_slots_) {
				for (size_t i = 0; i < old_cap; ++i) {
					if (IsFull(old_ctrl_[i])) {
						insert_or_assign(old_slots_[i].first, std::move(old_slots_[i].second));
						std::destroy_at(old_slots_ + i);
					}
				}
				::operator delete(old_ctrl_);
				::operator delete(old_slots_);
			}
		}

		void destroy_all() {
			if (!slots_) return;
			for (size_t i = 0; i < capacity_; ++i) {
				if (IsFull(ctrl_[i])) {
					std::destroy_at(slots_ + i);
				}
			}
			::operator delete(ctrl_);
			::operator delete(slots_);
			ctrl_ = nullptr;
			slots_ = nullptr;
			capacity_ = 0;
			size_ = 0;
		}


             };


} // end namespace mini_absl