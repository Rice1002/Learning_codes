#include <iostream>
using namespace std;

// 6k±1 试除判素数
bool isPrime(int n) {
	if (n < 2) return false;
	if (n % 2 == 0) return n == 2;
	if (n % 3 == 0) return n == 3;
	for (int i = 5; i <= n / i; i += 6)
		if (n % i == 0 || n % (i + 2) == 0) return false;
	return true;
}

// 由前半部分 half 镜像生成长度为 len 的回文数
// 例如 makePal(13, 3) = 131，makePal(123, 5) = 12321
int makePal(int half, int len) {
	int x = half;
	int h = (len & 1) ? half / 10 : half;  // 奇数位：中间位只出现一次
	while (h > 0) {
		x = x * 10 + h % 10;
		h /= 10;
	}
	return x;
}

int main() {
	int a, b;
	cin >> a >> b;

	// 一位回文素数 5、7 和唯一的偶数位回文素数 11
	int specials[] = { 5, 7, 11 };
	for (int x : specials)
		if (x >= a && x <= b)
			cout << x << '\n';

	// 只生成奇数位回文：3、5、7 位
	for (int len = 3; len <= 7; len += 2) {
		int half_len = (len + 1) / 2;
		int start = 1;
		for (int i = 1; i < half_len; i++) start *= 10;
		int end = start * 10;

		for (int half = start; half < end; half++) {
			int x = makePal(half, len);
			if (x < a) continue;
			if (x > b) return 0;   // 后续 half 更大，x 一定更大，可整体结束
			if (isPrime(x)) cout << x << '\n';
		}
	}
	return 0;
}