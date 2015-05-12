#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>

class LinearInterpolation {
private:
	cv::Mat_<double> X;
	cv::Mat_<double> Y;
	double alpha;

public:
	LinearInterpolation(const cv::Mat_<double>& X, const cv::Mat_<double>& Y, double alpha);
	cv::Mat_<double> predict(const cv::Mat_<double>& x);
};

