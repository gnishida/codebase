#include "NearestNeighborRegression.h"

NearestNeighborRegression::NearestNeighborRegression(const cv::Mat_<double>& X, const cv::Mat_<double>& Y) {
	this->X = X;
	this->Y = Y;
}

cv::Mat_<double> NearestNeighborRegression::predict(const cv::Mat_<double>& x, double& dist) {
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



