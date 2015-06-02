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
void shuffle(cv::Mat_<double>& data);
bool loadDataset(char* filename, cv::Mat_<double>& X);
void saveDataset(char* filename, const cv::Mat_<double>& mat);
void normalizeDataset(cv::Mat_<double> mat, cv::Mat_<double>& normalized_mat, cv::Mat_<double>& mean, cv::Mat_<double>& stddev);
void normalizeDataset2(cv::Mat_<double> mat, cv::Mat_<double>& normalized_mat, cv::Mat_<double>& mean, cv::Mat_<double>& stddev);
void addBias(cv::Mat& data);
void quadratic_dataset(const cv::Mat_<double>& data, cv::Mat_<double>& data2);
cv::Mat mat_square(const cv::Mat& m);
double mat_sum(const cv::Mat_<double>& m);
void mat_clamp(cv::Mat_<double>& m, double min_val, double max_val);
double correlation(const cv::Mat_<double>& m1, const cv::Mat_<double>& m2);
void meanStdDev(const cv::Mat_<double>& src, cv::Mat_<double>& mean, cv::Mat_<double>& stddev);
double rmse(const cv::Mat_<double>& trueData, const cv::Mat_<double>& predData, bool averageColumns);

void mat_save(char* filename, const cv::Mat_<double>& mat, bool normalize);

template<typename T>
T sqr(T val) { return val * val; }

// random
float genRand();
float genRand(float a, float b);
float genRandNormal(float mean, float variance);

template<typename T>
int sampleFromCdf(const std::vector<T> &cdf) {
	float rnd = genRand(0, cdf.back());

	for (int i = 0; i < cdf.size(); ++i) {
		if (rnd <= cdf[i]) return i;
	}

	return cdf.size() - 1;
}

template<typename T>
int sampleFromPdf(const std::vector<T> &pdf) {
	if (pdf.size() == 0) return 0;

	std::vector<T> cdf(pdf.size(), 0.0f);
	cdf[0] = pdf[0];
	for (int i = 1; i < pdf.size(); ++i) {
		if (pdf[i] >= 0) {
			cdf[i] = cdf[i - 1] + pdf[i];
		} else {
			cdf[i] = cdf[i - 1];
		}
	}

	return sampleFromCdf(cdf);
}

}