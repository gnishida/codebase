#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>

class LocalLinearRegression {
public:
	cv::Mat_<double> inputs;
	cv::Mat_<double> Y;
	int N;	// データ数
	double sigma;

public:
	LocalLinearRegression(const cv::Mat_<double>& inputs, const cv::Mat_<double>& Y, double sigma);
	cv::Mat_<double> predict(const cv::Mat_<double>& x);
};

