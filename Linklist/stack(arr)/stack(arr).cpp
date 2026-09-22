
#include <iostream>
#include <stdexcept>

template <typename T>
class SqStack
{
private:
	T* base;        // 栈底指针
	T* top;         // 栈顶指针（指向栈顶元素的下一个位置）
	int  stacksize;   // 当前容量

	void expand()     // 扩容：容量翻倍
	{
		int newSize = stacksize * 2;
		T* newBase = new T[newSize];
		for (int i = 0; i < stacksize; ++i)
			newBase[i] = base[i];
		delete[] base;
		base = newBase;
		top = base + stacksize;   // 栈长不变，top 重新定位
		stacksize = newSize;
	}

public:
	// 构造：默认容量 100
	explicit SqStack(int size = 100)
		: base(new T[size]), top(base), stacksize(size) {
	}

	// 析构
	~SqStack() { delete[] base; }

	SqStack(SqStack&& other) noexcept
		: base(other.base), top(other.top), stacksize(other.stacksize)
	{
		other.base = other.top = nullptr;
		other.stacksize = 0;
	}

	SqStack& operator=(SqStack&& other) noexcept
	{
		if (this != &other)
		{
			delete[] base;              // 释放自己原有的内存
			base = other.base;     // 偷对方的资源
			top = other.top;
			stacksize = other.stacksize;
			other.base = other.top = nullptr;
			other.stacksize = 0;
		}
		return *this;
	}

	bool empty() const { return top == base; }
	int  size()  const { return static_cast<int>(top - base); }

	// 取栈顶（不弹出）
	bool getTop(T& e) const
	{
		if (top == base) return false;
		e = *(top - 1);
		return true;
	}

	// 入栈
	bool push(const T& e)
	{
		if (top - base >= stacksize)
			expand();
		*top++ = e;
		return true;
	}

	// 出栈
	bool pop(T& e)
	{
		if (top == base) return false;
		e = *(--top);
		return true;
	}

	// 清空（保留内存）
	void clear() { top = base; }

	// 遍历输出（底 → 顶）
	void traverse() const
	{
		for (T* p = base; p < top; ++p)
			std::cout << *p << " ";
		std::cout << std::endl;
	}
};

using namespace std;

int main()
{
	// ---------- 1. 基础操作 ----------
	cout << "===== 基础操作 =====" << endl;
	{
		SqStack<int> s;
		int e = 0;
		cout << "初始 empty=" << (s.empty() ? "true" : "false")
			<< " (期望 true), size=" << s.size() << " (期望 0)" << endl;

		for (int i = 1; i <= 5; ++i) s.push(i * 10);
		cout << "push 10..50 后 size=" << s.size() << " (期望 5)" << endl;

		s.getTop(e);
		cout << "getTop=" << e << " (期望 50)" << endl;
		cout << "traverse(底->顶): "; s.traverse();

		s.pop(e);
		cout << "pop=" << e << " (期望 50), size=" << s.size() << " (期望 4)" << endl;

		cout << "全部出栈: ";
		while (!s.empty()) { s.pop(e); cout << e << " "; }
		cout << endl;

		cout << "空栈 getTop=" << (s.getTop(e) ? "true" : "false") << " (期望 false)" << endl;
		cout << "空栈 pop   =" << (s.pop(e) ? "true" : "false") << " (期望 false)" << endl;
	}

	// ---------- 2. 扩容 ----------
	cout << "\n===== 扩容 =====" << endl;
	{
		SqStack<int> s(5);          // 初始容量 5
		for (int i = 1; i <= 200; ++i) s.push(i);
		cout << "push 1..200 后 size=" << s.size() << " (期望 200)" << endl;

		int e = 0;
		s.getTop(e);
		cout << "getTop=" << e << " (期望 200)" << endl;

		cout << "弹出前 10 个: ";
		for (int i = 0; i < 10; ++i) { s.pop(e); cout << e << " "; }
		cout << "\n (期望 200 199 198 197 196 195 194 193 192 191)" << endl;
	}

	// ---------- 3. 清空 ----------
	cout << "\n===== 清空 =====" << endl;
	{
		SqStack<int> s;
		for (int i = 1; i <= 20; ++i) s.push(i);
		s.clear();
		cout << "clear 后 empty=" << (s.empty() ? "true" : "false")
			<< " (期望 true), size=" << s.size() << " (期望 0)" << endl;

		s.push(777);
		int e = 0;
		s.getTop(e);
		cout << "清空后再 push 777, getTop=" << e << " (期望 777)" << endl;
		s.clear();
	}

	// ---------- 4. 移动语义 ----------
	cout << "\n===== 移动语义 =====" << endl;
	{
		SqStack<int> s;
		for (int i = 1; i <= 5; ++i) s.push(i * 10);

		SqStack<int> moved = std::move(s);
		cout << "[移动构造] 源 empty=" << (s.empty() ? "true" : "false")
			<< " (期望 true), 源 size=" << s.size() << " (期望 0)" << endl;
		cout << "           新 size=" << moved.size() << " (期望 5), 内容: ";
		moved.traverse();

		SqStack<int> target;
		target.push(999);
		target = std::move(moved);
		cout << "[移动赋值] 源 empty=" << (moved.empty() ? "true" : "false")
			<< " (期望 true), 目标 size=" << target.size() << " (期望 5)" << endl;
		cout << "           目标内容: "; target.traverse();

		target = std::move(target);   // 自移动
		cout << "[自移动赋值] size=" << target.size() << " (期望 5)" << endl;
	}

	// ---------- 5. string 类型 ----------
	cout << "\n===== string 类型 =====" << endl;
	{
		SqStack<string> s;
		s.push("hello");
		s.push("world");
		s.push("C++");
		cout << "traverse: "; s.traverse();

		string e;
		s.pop(e);
		cout << "pop=" << e << " (期望 C++)" << endl;
		s.getTop(e);
		cout << "getTop=" << e << " (期望 world)" << endl;
	}

	// ---------- 6. double 类型 ----------
	cout << "\n===== double 类型 =====" << endl;
	{
		SqStack<double> s;
		s.push(1.5);
		s.push(2.5);
		s.push(3.5);
		cout << "traverse: "; s.traverse();

		double e = 0;
		s.pop(e);
		cout << "pop=" << e << " (期望 3.5)" << endl;
	}

	cout << "\n顺序栈全部测试结束。" << endl;
	return 0;
}
