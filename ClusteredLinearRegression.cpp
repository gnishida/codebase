#include "ClusteredLinearRegression.h"
#include "MLUtils.h"

using namespace std;

ClusteredLinearRegression::ClusteredLinearRegression(const cv::Mat_<double>& X, const cv::Mat_<double>& Y) {
	int N = X.rows;

	vector<cv::Mat_<float> > clusterX, clusterY;
	vector<cv::Mat_<float> > floatCentroids;
	{
		cv::Mat floatX, floatY;
		X.convertTo(floatX, CV_32F);
		Y.convertTo(floatY, CV_32F);
		
		cv::Mat_<float> centroid;
		X.convertTo(centroid, CV_32F);
		cv::reduce(centroid, centroid, 0, CV_REDUCE_AVG);
		partition(floatX, floatY, centroid, 20, clusterX, clusterY, floatCentroids);
	}

	clusterCentroids.resize(clusterX.size());
	W.resize(clusterX.size());

	for (int i = 0; i < clusterX.size(); ++i) {
		floatCentroids[i].convertTo(clusterCentroids[i], CV_64F);

		cv::Mat_<double> X2;
		clusterX[i].convertTo(X2, CV_64F);
		ml::addBias(X2);
		cv::Mat_<double> Y2;
		clusterY[i].convertTo(Y2, CV_64F);

		W[i] = X2.inv(cv::DECOMP_SVD) * Y2;
	}
}

cv::Mat_<double> ClusteredLinearRegression::predict(const cv::Mat_<double>& x) {
	// 直近のクラスタを探す
	float min_dist = std::numeric_limits<float>::max();
	int min_id = -1;
	for (int i = 0; i < clusterCentroids.size(); ++i) {
		float dist = cv::norm((clusterCentroids[i] - x));
		if (dist < min_dist) {
			min_dist = dist;
			min_id = i;
		}
	}

	cv::Mat_<double> x2 = x.clone();
	ml::addBias(x2);
	return x2 * W[min_id];
}


/**
 * データXをk-meansクラスタリングで階層的に分割していく。
 * それに合わせて、データYも分割する。
 * データX、データYの行数は同じであること。
 *
 * @param X						データX
 * @param Y						データY
 * @param indices				各データ要素の、元データでのindex番号
 * @param minSize				クラスタの最小サイズ
 * @param clusterX [OUT]		データXのクラスタリング結果
 * @param clusterY [OUT]		データYのクラスタリング結果
 * @param clusterCentroids [OUT]	クラスタリング結果における各要素の、元データでのindex番号
 */
void ClusteredLinearRegression::partition(const cv::Mat_<float>& X, const cv::Mat_<float>& Y, const cv::Mat_<float>& centroid, int minSize, vector<cv::Mat_<float> >&clusterX, vector<cv::Mat_<float> >&clusterY, vector<cv::Mat_<float> >& clusterCentroids) {
	// サンプル数がminSize未満になるまで、繰り返し、X、Yを分割する。
	cv::Mat samples;
	X.convertTo(samples, CV_32F);
	cv::Mat_<float> centroids;
	cv::Mat labels;
	//cv::TermCriteria cri(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 50, FLT_EPSILON);
	cv::TermCriteria cri(cv::TermCriteria::COUNT, 200, FLT_EPSILON);
	double compactness = cv::kmeans(samples, 2, labels, cri, 200, cv::KMEANS_PP_CENTERS, centroids);
		
	int nClass1 = cv::countNonZero(labels);
	int nClass0 = X.rows - nClass1;

	if (nClass1 < minSize || nClass0 < minSize) {
		clusterX.push_back(X);
		clusterY.push_back(Y);
		clusterCentroids.push_back(centroid); 
		return;
	}

	cv::Mat_<float> classX0(nClass0, X.cols);
	cv::Mat_<float> classX1(nClass1, X.cols);
	cv::Mat_<float> classY0(nClass0, Y.cols);
	cv::Mat_<float> classY1(nClass1, Y.cols);

	int index0 = 0;
	int index1 = 0;
	for (int r = 0; r < X.rows; ++r) {
		if (labels.at<float>(r, 0) == 0) {
			for (int c = 0; c < X.cols; ++c) {
				classX0(index0, c) = X(r, c);
			}
			for (int c = 0; c < Y.cols; ++c) {
				classY0(index0, c) = Y(r, c);
			}

			index0++;
		} else {
			for (int c = 0; c < X.cols; ++c) {
				classX1(index1, c) = X(r, c);
			}
			for (int c = 0; c < Y.cols; ++c) {
				classY1(index1, c) = Y(r, c);
			}

			index1++;
		}
	}

	partition(classX0, classY0, centroids.row(0), minSize, clusterX, clusterY, clusterCentroids);
	partition(classX1, classY1, centroids.row(1), minSize, clusterX, clusterY, clusterCentroids);
}

