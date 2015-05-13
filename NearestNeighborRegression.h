#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>

class NearestNeighborRegression {
public:
	NearestNeighborRegression();
	cv::Mat_<double> predict(const cv::Mat_<double>& X, const cv::Mat_<double>& Y, const cv::Mat_<double>& x, double& dist);
};

