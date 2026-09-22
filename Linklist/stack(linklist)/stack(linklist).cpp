#ifndef LINKSTACK_H
#define LINKSTACK_H

#include <iostream>

template <typename T>
class LinkStack
{
private:
	struct Node
	{
		T     data;
		Node* next;
		Node(const T& d, Node* n = nullptr) : data(d), next(n) {}
	};

	Node* top;    // 栈顶指针（指向栈顶结点）
	int   count;  // 元素个数

public:
	LinkStack() : top(nullptr), count(0) {}

	~LinkStack() { clear(); }

	// 禁用拷贝
	LinkStack(const LinkStack&) = delete;
	LinkStack& operator=(const LinkStack&) = delete;

	bool empty() const { return top == nullptr; }
	int  size()  const { return count; }

	bool getTop(T& e) const
	{
		if (!top) return false;
		e = top->data;
		return true;
	}

	bool push(const T& e)
	{
		top = new Node(e, top);   // 头插
		++count;
		return true;
	}

	bool pop(T& e)
	{
		if (!top) return false;
		Node* p = top;
		e = p->data;
		top = top->next;
		delete p;
		--count;
		return true;
	}

	// 清空（释放所有结点）
	void clear()
	{
		while (top)
		{
			Node* p = top;
			top = top->next;
			delete p;
		}
		count = 0;
	}

	// 遍历输出（顶 → 底）
	void traverse() const
	{
		for (Node* p = top; p; p = p->next)
			std::cout << p->data << " ";
		std::cout << std::endl;
	}
};

#endif