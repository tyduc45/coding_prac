#include<iostream>
//#define base_2_derive
//#define derive_2_base
//#define safe_derive_cast_flag
//#define type_id_intro
#define dynamic_cast_learn

#ifdef base_2_derive
struct Base
{

	int i = 1;
};

struct Derived : Base
{
	int j = 2;
};

struct Derived1 : Base {
	double k = 3.0;
};

int main()
{
	Base* b = new Base;
	Derived* d = b;  //错误，无法覆盖子类新增数据变量
	return 0;
}
#endif

#ifdef derive_2_base
struct Base
{
	int i = 1;
};

struct Derived : Base
{
	int j = 2;
};

struct Derived1 : Base {
	double k = 3.0;
};

int main()
{
	Base* b = new Derived;
	//Derived* d = b; 错误， 因为编译器不知道上下文，因此不能确定b是否真的是derive而不是base ， 如果编译器允许就会有上面base_2_derive情况的风险
	Derived* d = static_cast<Derived*>(b); // 这是对的，前提是你需要保证d真的是Derived，而不是base ，否则会出UB，实际运行时出随机值，static_cast 就是允许基类向派生类转化而生的
	std::cout << d->j << "\n";
	return 0;
}
#endif

#ifdef safe_derive_cast_flag
struct Base
{
	int flag = 0;
	int i = 1;
};

struct Derived : Base
{
	int j = 2;
	Derived() {
		flag = 1;
	}
};

struct Derived1 : Derived {
	Derived1() { 
		flag = 2;
	}
	double k = 3.0;
};

int main()
{
	Base* b = new Derived1;
	//如何判断是否真的时基类->派生类的情况？可以用flag变量标记来区分当前基类和不同子类的对象指针b
	if (b->flag == 1) {
		std::cout << "is derived , can cast\n";
		Derived* d = static_cast<Derived*>(b);
	}
	if (b->flag == 2) {
		std::cout << "is derived1 , can not cast\n"; return -1;
		Derived1* d = static_cast<Derived1*>(b);
	}
	return 0;
}
#endif

#ifdef type_id_intro
#include <typeinfo>
struct Base
{
	const std::type_info *type = &typeid(Base);
	int i = 1;
};

struct Derived : Base
{
	int j = 2;
	Derived() {
		type = &typeid(Derived);
	}
};

struct Derived1 : Base {
	Derived1() {
		type = &typeid(Derived1);
	}
	double k = 3.0;
};

int main()
{
	Base* b = new Derived;
	Base* c = new Derived;
	if (b->type == c->type) std::cout << "同一类对象的typeid 完全一致 \n";

	//如何判断是否真的是基类->派生类的情况?除了type还能用type id,为每个类分配全局唯一标识符
	if (b->type == &typeid(Derived)) {
		std::cout << "is derived , can cast\n";
		Derived* d = static_cast<Derived*>(b);
	}
	if (b->type == &typeid(Derived1)) {
		std::cout << "is derived1 , can not cast\n"; return -1;
		Derived1* d = static_cast<Derived1*>(b);
	}
	return 0;
}
#endif 

#ifdef dynamic_cast_learn
#include <typeinfo>
struct Base
{
	//compiler add vptr automatically void *vptr
	int i = 1;
	virtual void v() {};
};

struct Derived : Base
{
	int j = 2;
	
};

struct Derived1 : Base {
	
	double k = 3.0;
};

int main()
{
	// dynamic cast 就是官方把上述过程自动化了，但是官方的实现是借助虚表，虚表是存储当前虚函数地址的一个表，dynamic cast 直接把typeinfo存在里面
	// 因此dynamic cast依旧是服务向派生类转化的场景的，适用于编译期知道类型属于同一继承体系，但运行时不确定这个 Base* / Base& 实际指向哪个派生类对象的场景。
	Base* b = new Derived1;
	Base* b1 = new Derived;
	// Derived *d = dynamic_cast<Derived*>(b); 假设你的类不是多态类，那么这么写错了，c++有两种类， 有虚函数的（多态类）和没有虚函数的， 
	// dynamic_cast要求必须是多态类，因为前面说到他需要把typeinfo存到虚表
	Derived* d = dynamic_cast<Derived*>(b);
	if (d == nullptr) { std::cout << "cast failed :b\n"; } // 如果转换失败就是返回nullptr，所以一定要做转换
	Derived1* d1 = dynamic_cast<Derived1*>(b1);
	if (d1 == nullptr) { std::cout << "cast failed :b1\n"; }
	return 0;

}
#endif