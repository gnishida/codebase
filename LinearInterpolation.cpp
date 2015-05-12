#include "LinearInterpolation.h"
#include "MLUtils.h"

using namespace std;

/**
 * Linear interpolationを初期化する。
 *
 * @param X		サンプルデータの入力X
 * @param Y		サンプルデータの出力Y
 * @param alpha	どの程度の近傍点をinterpolationとしｔ使用するか (最低1。2ぐらいが適当か)
 */
LinearInterpolation::LinearInterpolation(const cv::Mat_<double>& X, const cv::Mat_<double>& Y, double alpha) {
	this->X = X.clone();
	this->Y = Y.clone();
	this->alpha = alpha;
}

/**
 * Linear interpolationにより、指定されたデータxに対応するyを計算する。
 *
 * @param x		データポイント
 * @return		対応する値
 */
cv::Mat_<double> LinearInterpolation::predict(const cv::Mat_<double>& x) {
	// データの次元数
	int D = X.cols;

	map<double, int> lengths;
	for (int r = 0; r < X.rows; ++r) {
		double dist = cv::norm(X.row(r) - x);
		lengths[dist] = r;
	}

	// 使用する近傍データの数
	int N = (D + 1) * alpha;

	cv::Mat_<double> NeighborX(N, X.cols);
	cv::Mat_<double> NeighborY(N, Y.cols);

	int count = 0;
	for (auto it = lengths.begin(); it != lengths.end() && count < N; ++it, ++count) {
		X.row(it->second).copyTo(NeighborX.row(count));
		Y.row(it->second).copyTo(NeighborY.row(count));
	}

	ml::addBias(NeighborX);

	cv::Mat_<double> x2 = x.clone();
	ml::addBias(x2);

	return x2 * NeighborX.inv(cv::DECOMP_SVD) * NeighborY;
}