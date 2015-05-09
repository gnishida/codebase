#include "LinearRegressionRegularization.h"

using namespace std;

LinearRegressionRegularization::LinearRegressionRegularization() {
}


double LinearRegressionRegularization::train(const cv::Mat_<double>& X, const cv::Mat_<double>& Y, double lambda, double alpha, int maxIter) {
	//W = X.inv(cv::DECOMP_SVD) * Y;
	W = cv::Mat_<double>::zeros(X.cols, Y.cols);

	for (int iter = 0; iter < maxIter; ++iter) {
		cv::Mat_<double> dW = X.t() * (X * W - Y) / X.rows + lambda * W;

		W -= dW * alpha;
	}

	// residueの計算
	cv::Mat_<double> avg_mat;
	cv::reduce((X * W - Y).mul(X * W - Y), avg_mat, 0, CV_REDUCE_AVG);
	cv::reduce(avg_mat, avg_mat, 1, CV_REDUCE_SUM);
	return sqrt(avg_mat(0, 0));
}

cv::Mat LinearRegressionRegularization::predict(const cv::Mat_<double>& x) {
	return x * W;
}

double LinearRegressionRegularization::conditionNumber() {
	cv::Mat_<double> w1, w2, u, vt;
	cv::SVD::compute(W, w1, u, vt);

	cv::SVD::compute(W.inv(cv::DECOMP_SVD), w2, u, vt);

	return w1(0,0) * w2(0, 0);
}

