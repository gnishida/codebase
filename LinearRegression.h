#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>

class LinearRegression {
public:
	cv::Mat_<double> W;

public:
	LinearRegression();

	double train(const cv::Mat_<double>& inputs, const cv::Mat_<double>& Y);
	cv::Mat predict(const cv::Mat_<double>& inputs);
	double conditionNumber();
};

