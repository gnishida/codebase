#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>

using namespace std;

class ClusteredLinearRegression {
private:
	vector<cv::Mat_<double> > clusterCentroids;
	vector<cv::Mat_<double> > W;

public:
	ClusteredLinearRegression(const cv::Mat_<double>& X, const cv::Mat_<double>& Y);

public:
	cv::Mat_<double> predict(const cv::Mat_<double>& x);

private:
	void partition(const cv::Mat_<float>& X, const cv::Mat_<float>& Y, const cv::Mat_<float>& centroid, int minSize, vector<cv::Mat_<float> >&clusterX, vector<cv::Mat_<float> >&clusterY, vector<cv::Mat_<float> >& clusterCentroids);
};

