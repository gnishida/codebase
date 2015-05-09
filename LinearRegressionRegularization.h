#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>

class LinearRegressionRegularization {
public:
	cv::Mat_<double> W;

public:
	LinearRegressionRegularization();

	double train(const cv::Mat_<double>& X, const cv::Mat_<double>& Y, double lambda, double alpha, int maxIter);
	cv::Mat predict(const cv::Mat_<double>& x);
	double conditionNumber();
};

