#pragma once

#include <vector>

// random
void initRand(int seed);
float genRand();
float genRand(float a, float b);
float genRandNormal(float mean, float variance);
float genRandInt(float a, float b, int num);

// statistics
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

template<typename T>
T sum(const std::vector<T>& vec) {
	T s = 0;
	for (int i = 0; i < vec.size(); ++i) {
		s += vec[i];
	}
	return s;
}

template<typename T>
T mean(const std::vector<T>& vec) {
	return sum(vec) / vec.size();
}

template<typename T>
T variance(const std::vector<T>& vec) {
	T avg = mean(vec);
	T s = 0;
	for (int i = 0; i < vec.size(); ++i) {
		s += (vec[i] - avg) * (vec[i] - avg);
	}
	return s / vec.size();
}

double log(double val, double base);
