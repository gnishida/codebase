#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

namespace ml {

std::vector<std::string> splitDataset(const std::string &str, char sep);
void splitDataset(const cv::Mat_<double>& data, float ratio1, cv::Mat_<double>& data1, cv::Mat_<double>& data2);
void splitDataset(const cv::Mat_<double>& data, float ratio1, float ratio2, cv::Mat_<double>& data1, cv::Mat_<double>& data2, cv::Mat_<double>& data3);
bool loadDataset(char* filename, cv::Mat_<double>& X);
void saveDataset(char* filename, const cv::Mat_<double>& mat);
void normalizeDataset(cv::Mat_<double> mat, cv::Mat_<double>& normalized_mat, cv::Mat_<double>& mean, cv::Mat_<double>& stddev);
void normalizeDataset2(cv::Mat_<double> mat, cv::Mat_<double>& normalized_mat, cv::Mat_<double>& mean, cv::Mat_<double>& stddev);
void addBias(cv::Mat& data);
cv::Mat mat_square(const cv::Mat& m);
double mat_sum(const cv::Mat_<double>& m);
double correlation(const cv::Mat_<double>& m1, const cv::Mat_<double>& m2);
void meanStdDev(const cv::Mat_<double>& src, cv::Mat_<double>& mean, cv::Mat_<double>& stddev);
double rmse(const cv::Mat_<double>& trueData, const cv::Mat_<double>& predData);

template<typename T>
T sqr(T val) { return val * val; }

}