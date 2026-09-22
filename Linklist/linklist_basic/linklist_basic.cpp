#include<iostream>

template <typename T>
class Linklist {
private:
	struct Node {
		T data;
		Node* next;

		Node(const T& value, Node* nextNode = nullptr)
			: data(value), next(nextNode) {
		}
		Node(T&& value,Node* nextNode = nullptr)
			:data(std::move(value)),next(nextNode){ }
	};
}