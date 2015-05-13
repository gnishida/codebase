#include "NearestNeighborRegression.h"

NearestNeighborRegression::NearestNeighborRegression() {
}

/**
 * Nearest neighborを使って、xに対応するyを探して返却する。
 *
 * @param X				データX
 * @param Y				データY
 * @param x				データx
 * @param dist [OUT]	nearest neighborまでの距離
 * @return				yの予測値
 */
cv::Mat_<double> NearestNeighborRegression::predict(const cv::Mat_<double>& X, const cv::Mat_<double>& Y, const cv::Mat_<double>& x, double& dist) {
	int best_index;
	dist = std::numeric_limits<double>::max();
	for (int r = 0; r < X.rows; ++r) {
		double d = cv::norm(X.row(r) - x);
		if (d < dist) {
			dist = d;
			best_index = r;
		}
	}

	return Y.row(best_index);
}



