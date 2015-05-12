#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>

class NearestNeighborRegression {
private:
	cv::Mat_<double> X;
	cv::Mat_<double> Y;

public:
	NearestNeighborRegression(const cv::Mat_<double>& X, const cv::Mat_<double>& Y);
	cv::Mat_<double> predict(const cv::Mat_<double>& x, double& dist);
};

