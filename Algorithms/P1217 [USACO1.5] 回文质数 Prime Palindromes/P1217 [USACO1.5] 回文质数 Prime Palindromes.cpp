#include <iostream>
#include <vector>
using namespace std;

int main() {
	int n;
	cin >> n;

	// notPrime[i] = true 表示 i 不是素数
	vector<bool> notPrime(n + 1, false);

	// 0 和 1 不是素数
	if (n >= 0) notPrime[0] = true;
	if (n >= 1) notPrime[1] = true;

	// 只需筛到 √n
	for (int i = 2; i <= n / i; i++) {
		if (!notPrime[i]) {                    // i 是素数
			// 从 i*i 开始，把 i 的倍数全部标记为合数
			for (int j = i * i; j <= n; j += i)
				notPrime[j] = true;
		}
	}

	// 输出所有素数
	for (int i = 2; i <= n; i++)
		if (!notPrime[i])
			cout << i << '\n';

	return 0;
}