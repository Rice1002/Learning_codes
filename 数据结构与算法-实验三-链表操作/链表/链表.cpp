#include <memory>
#include <concepts>
#include <iterator>
#include <utility>
#include <initializer_list>
#include <cstddef>
#include <type_traits>
#include <iostream>

template <typename T>
	requires std::is_object_v<T>
class ForwardList {
	struct Node {
		T data;
		std::unique_ptr<Node> next;

		template <typename... Args>
		explicit Node(Args&&... args)
			: data(std::forward<Args>(args)...) {
		}
	};

	std::unique_ptr<Node> head_;
	Node* tail_ = nullptr;        // 观察指针，不拥有
	std::size_t size_ = 0;

public:
	// ---------- 迭代器：一个模板出 const / non-const 两版 ----------
	template <bool Const>
	class Iterator {
		using NodePtr = std::conditional_t<Const, const Node*, Node*>;
		NodePtr cur_ = nullptr;

		friend class ForwardList;
		explicit Iterator(NodePtr n) noexcept : cur_(n) {}

	public:
		using iterator_concept = std::forward_iterator_tag;
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::remove_cv_t<T>;
		using difference_type = std::ptrdiff_t;
		using pointer = std::conditional_t<Const, const T*, T*>;
		using reference = std::conditional_t<Const, const T&, T&>;

		Iterator() noexcept = default;

		// non-const -> const 的隐式转换
		template <bool C>
			requires (C == Const || Const)
		Iterator(const Iterator<C>& other) noexcept : cur_(other.cur_) {}

		reference operator*()  const noexcept { return cur_->data; }
		pointer   operator->() const noexcept { return std::addressof(cur_->data); }

		Iterator& operator++() noexcept { cur_ = cur_->next.get(); return *this; }
		Iterator  operator++(int) noexcept { auto t = *this; ++*this; return t; }

		friend bool operator==(const Iterator&, const Iterator&) noexcept = default;
	};

	using iterator = Iterator<false>;
	using const_iterator = Iterator<true>;

	// ---------- 构造 / 析构 ----------
	ForwardList() noexcept = default;

	ForwardList(std::initializer_list<T> init) {
		for (const auto& v : init) push_back(v);
	}

	// 迭代式释放，避免递归爆栈
	~ForwardList() { clear(); }

	ForwardList(const ForwardList& other) {
		for (const auto& v : other) push_back(v);
	}

	ForwardList(ForwardList&& other) noexcept
		: head_(std::move(other.head_)),
		tail_(other.tail_),
		size_(other.size_) {
		other.tail_ = nullptr;
		other.size_ = 0;
	}

	// copy-and-swap，一箭双雕
	ForwardList& operator=(ForwardList other) noexcept {
		swap(other);
		return *this;
	}

	void swap(ForwardList& other) noexcept {
		std::swap(head_, other.head_);
		std::swap(tail_, other.tail_);
		std::swap(size_, other.size_);
	}

	// ---------- 容量 ----------
	[[nodiscard]] bool empty() const noexcept { return size_ == 0; }
	[[nodiscard]] std::size_t size() const noexcept { return size_; }

	// ---------- 访问 ----------
	[[nodiscard]] T& front()       noexcept { return head_->data; }
	[[nodiscard]] const T& front() const noexcept { return head_->data; }
	[[nodiscard]] T& back()        noexcept { return tail_->data; }
	[[nodiscard]] const T& back()  const noexcept { return tail_->data; }

	// ---------- 插入 ----------
	template <typename... Args>
	T& emplace_front(Args&&... args) {
		auto node = std::make_unique<Node>(std::forward<Args>(args)...);
		node->next = std::move(head_);
		head_ = std::move(node);
		if (!tail_) tail_ = head_.get();
		++size_;
		return head_->data;
	}

	template <typename... Args>
	T& emplace_back(Args&&... args) {
		auto node = std::make_unique<Node>(std::forward<Args>(args)...);
		Node* raw = node.get();
		if (tail_) { tail_->next = std::move(node); tail_ = raw; }
		else { head_ = std::move(node);       tail_ = raw; }
		++size_;
		return raw->data;
	}

	void push_front(const T& v) { emplace_front(v); }
	void push_front(T&& v) { emplace_front(std::move(v)); }
	void push_back(const T& v) { emplace_back(v); }
	void push_back(T&& v) { emplace_back(std::move(v)); }

	// ---------- 删除 ----------
	void pop_front() noexcept {
		if (!head_) return;
		head_ = std::move(head_->next);
		if (!head_) tail_ = nullptr;
		--size_;
	}

	void clear() noexcept {
		while (head_) head_ = std::move(head_->next);
		tail_ = nullptr;
		size_ = 0;
	}

	// ---------- 反转 ----------
	void reverse() noexcept {
		Node* prev = nullptr;
		Node* cur = head_.release();
		tail_ = cur;
		while (cur) {
			Node* next = cur->next.release();
			cur->next.reset(prev);
			prev = cur;
			cur = next;
		}
		head_.reset(prev);
	}

	// ---------- 归并排序：全程 unique_ptr + move ----------
	void sort() noexcept {
		head_ = mergeSort(std::move(head_));
		tail_ = head_.get();
		if (tail_) while (tail_->next) tail_ = tail_->next.get();
	}

	// ---------- 迭代器 ----------
	[[nodiscard]] iterator       begin()  noexcept { return iterator(head_.get()); }
	[[nodiscard]] iterator       end()    noexcept { return iterator(nullptr); }
	[[nodiscard]] const_iterator begin()  const noexcept { return const_iterator(head_.get()); }
	[[nodiscard]] const_iterator end()    const noexcept { return const_iterator(nullptr); }
	[[nodiscard]] const_iterator cbegin() const noexcept { return begin(); }
	[[nodiscard]] const_iterator cend()   const noexcept { return end(); }

private:
	// 合并两个有序 unique_ptr 链，返回新头
	static std::unique_ptr<Node>
		merge(std::unique_ptr<Node> a, std::unique_ptr<Node> b) noexcept {
		std::unique_ptr<Node> head;
		std::unique_ptr<Node>* pp = &head;
		while (a && b) {
			std::unique_ptr<Node>& chosen = (a->data <= b->data) ? a : b;
			auto tmp = std::move(chosen);
			chosen = std::move(tmp->next);
			*pp = std::move(tmp);
			pp = &((*pp)->next);
		}
		*pp = a ? std::move(a) : std::move(b);
		return head;
	}

	static std::unique_ptr<Node>
		mergeSort(std::unique_ptr<Node> head) noexcept {
		if (!head || !head->next) return head;

		Node* slow = head.get();
		Node* fast = head->next.get();
		while (fast && fast->next) {
			slow = slow->next.get();
			fast = fast->next->next.get();
		}

		auto mid = std::move(slow->next);
		slow->next = nullptr;

		return merge(mergeSort(std::move(head)),
			mergeSort(std::move(mid)));
	}
};