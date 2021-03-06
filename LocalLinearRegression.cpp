﻿#include "LocalLinearRegression.h"
#include "MLUtils.h"

using namespace std;

LocalLinearRegression::LocalLinearRegression() {
}

cv::Mat_<double> LocalLinearRegression::predict(const cv::Mat_<double>& inputs, const cv::Mat_<double>& Y, const cv::Mat_<double>& x, double sigma) {
	int N = inputs.rows;

	// 重み行列を計算
	cv::Mat_<double> W = cv::Mat_<double>::zeros(N, N);
	for (int i = 0; i < N; ++i) {
		double dot_product = (inputs.row(i) - x).dot(inputs.row(i) - x);
		dot_product = dot_product / sigma / sigma;
		if (dot_product > 1) dot_product = 1;
		//W(i, i) = exp(-dot_product / 2.0 / sigma / sigma);
		W(i, i) = (1 - dot_product) * (1 - dot_product);
	}

	// Xを計算
	cv::Mat_<double> X = inputs - cv::repeat(x, N, 1);
	ml::addBias(X);

	double det = cv::determinant((X.t() * W * X));
	if (det == 0) {
		cout << "Determinant = 0" << endl;
	}
		
	cv::Mat_<double> alpha = (X.t() * W * X).inv(cv::DECOMP_SVD) * X.t() * W * Y;

	return alpha.row(alpha.rows - 1);
}
