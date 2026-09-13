#include<algorithm>
#include <vector>
#include <iostream>
using namespace std;
struct BigInt {
	vector<int> d;

	BigInt(long long x = 0) {
		if (x == 0) d.push_back(0);
		while (x > 0) {
			d.push_back(x % 10);
			x /= 10;
		}
	}
	BigInt operator+(const BigInt& other) const {
		BigInt res;
		res.d.clear();
		int carry = 0;
		int n = max(d.size(), other.d.size());
		for (int i = 0; i < n || carry; i++) {
			int sum = carry;
			if (i < (int)d.size()) sum += d[i];
			if (i < (int)other.d.size()) sum += other.d[i];
			res.d.push_back(sum % 10);
			carry = sum / 10;
		}
		return res;
	}
	BigInt operator*(int x) const {
		BigInt res;
		res.d.clear();
		int carry = 0;
		for (int i = 0; i < (int)d.size() || carry; i++) {
			int prod = carry;
			if (i < (int)d.size()) prod += d[i] * x;
			res.d.push_back(prod % 10);
			carry = prod / 10;
		}
		return res;
	}
	bool operator<(const BigInt& other) const {
		if (d.size() != other.d.size()) return d.size() < other.d.size();
		for (int i = (int)d.size() - 1; i >= 0; i--) {
			if (d[i] != other.d[i]) return d[i] < other.d[i];
		}
		return false;
	}
	friend ostream& operator<<(ostream& os, const BigInt& b) {
		for (int i = (int)b.d.size() - 1; i >= 0; i--) {
			os << b.d[i];
		}
		return os;
	}
};
int main()
{
	int n, m;
    cin >> n >> m;

    // 预处理 2 的幂：pow2[k] = 2^k
    vector<BigInt> pow2(m + 1);
    pow2[0] = BigInt(1);
    for (int i = 1; i <= m; i++) {
        pow2[i] = pow2[i - 1] * 2;
    }

    BigInt ans(0);

    for (int row = 0; row < n; row++) {
        vector<int> a(m);
        for (int j = 0; j < m; j++) {
            cin >> a[j];
        }

        // dp[i][j] 表示区间 [i, j] 取完的最大得分
        vector<vector<BigInt>> dp(m, vector<BigInt>(m));

        // 按区间长度从小到大 DP
        for (int len = 1; len <= m; len++) {
            for (int i = 0; i + len - 1 < m; i++) {
                int j = i + len - 1;
                if (len == 1) {
                    dp[i][j] = pow2[m] * a[i];
                } else {
                    BigInt factor = pow2[m - len + 1];
                    BigInt left  = factor * a[i] + dp[i + 1][j];
                    BigInt right = factor * a[j] + dp[i][j - 1];
                    dp[i][j] = (left < right) ? right : left;
                }
            }
        }

        ans = ans + dp[0][m - 1];
    }

    cout << ans << endl;
    return 0;
}

