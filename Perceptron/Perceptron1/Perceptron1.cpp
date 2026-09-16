#include <iostream>
#include <vector>
#include <cmath>

class LogisticRegression {
public:
	LogisticRegression(int n_features, double lr = 0.5, int epochs = 2000)
		: lr(lr), epochs(epochs) {
		w.assign(n_features, 0.0);
		b = 0.0;
	}

	// Sigmoid，数值稳定写法
	static double sigmoid(double z) {
		if (z >= 0.0) {
			return 1.0 / (1.0 + std::exp(-z));
		}
		else {
			double ez = std::exp(z);
			return ez / (1.0 + ez);
		}
	}

	// 返回概率 P(y=1 | x)
	double predict_proba(const std::vector<double>& x) const {
		double z = b;
		for (size_t i = 0; i < w.size(); ++i) z += w[i] * x[i];
		return sigmoid(z);
	}

	// 硬分类
	int predict(const std::vector<double>& x) const {
		return predict_proba(x) >= 0.5 ? 1 : 0;
	}

	void train(const std::vector<std::vector<double>>& X,
		const std::vector<int>& Y) {
		for (int epoch = 1; epoch <= epochs; ++epoch) {
			double total_loss = 0.0;
			for (size_t k = 0; k < X.size(); ++k) {
				double y_hat = predict_proba(X[k]);
				double t = static_cast<double>(Y[k]);

				// 累加交叉熵损失
				const double eps = 1e-12;
				total_loss += -(t * std::log(y_hat + eps)
					+ (1 - t) * std::log(1 - y_hat + eps));

				// 梯度：dL/dz = y_hat - t
				double err = y_hat - t;
				for (size_t i = 0; i < w.size(); ++i)
					w[i] -= lr * err * X[k][i];
				b -= lr * err;
			}
			if (epoch % 200 == 0)
				std::cout << "Epoch " << epoch
				<< "  loss = " << total_loss << "\n";
		}
	}

	void print_params() const {
		std::cout << "weights: ";
		for (double wi : w) std::cout << wi << " ";
		std::cout << "\nbias: " << b << "\n";
	}

private:
	std::vector<double> w;
	double b;
	double lr;
	int epochs;
};

int main() {
	// AND 真值表
	std::vector<std::vector<double>> X = {
		{0, 0}, {0, 1}, {1, 0}, {1, 1}
	};
	std::vector<int> Y = { 0, 0, 0, 1 };

	LogisticRegression model(2, 0.5, 2000);
	model.train(X, Y);

	std::cout << "\nLearned parameters:\n";
	model.print_params();

	std::cout << "\nTesting AND:\n";
	for (size_t i = 0; i < X.size(); ++i) {
		double p = model.predict_proba(X[i]);
		std::cout << X[i][0] << " AND " << X[i][1]
			<< " = " << model.predict(X[i])
			<< "   (P = " << p << ")\n";
	}
	return 0;
}