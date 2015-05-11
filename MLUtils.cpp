#include "MLUtils.h"

using namespace std;

namespace ml {

vector<string> splitDataset(const string &str, char sep) {
    vector<string> v;
    stringstream ss(str);
    string buffer;
    while (getline(ss, buffer, sep)) {
        v.push_back(buffer);
    }
    return v;
}

/**
 * データを、指定された比率に従い、2つのデータに分割する。
 *
 */
void splitDataset(const cv::Mat_<double>& data, float ratio1, cv::Mat_<double>& data1, cv::Mat_<double>& data2) {
	int rows1 = data.rows * ratio1;
	int rows2 = data.rows - rows1;

	data1 = cv::Mat_<double>(rows1, data.cols);
	data2 = cv::Mat_<double>(rows2, data.cols);

	for (int r = 0; r < data.rows; ++r) {
		for (int c = 0; c < data.cols; ++c) {
			if (r < rows1) {
				data1(r, c) = data(r, c);
			} else {
				data2(r - rows1, c) = data(r, c);
			}
		}
	}
}

void splitDataset(const cv::Mat_<double>& data, float ratio1, float ratio2, cv::Mat_<double>& data1, cv::Mat_<double>& data2, cv::Mat_<double>& data3) {
	int rows1 = data.rows * ratio1;
	int rows2 = data.rows * ratio2;
	int rows3 = data.rows - rows1 - rows2;

	data1 = cv::Mat_<double>(rows1, data.cols);
	data2 = cv::Mat_<double>(rows2, data.cols);
	data3 = cv::Mat_<double>(rows3, data.cols);

	for (int r = 0; r < data.rows; ++r) {
		for (int c = 0; c < data.cols; ++c) {
			if (r < rows1) {
				data1(r, c) = data(r, c);
			} else if (r < rows1 + rows2) {
				data2(r - rows1, c) = data(r, c);
			} else {
				data3(r - rows1 - rows2, c) = data(r, c);
			}
		}
	}
}

/**
 * ファイルからデータを読み込む。
 * ファイルの各行は、[x1, x2, ...],[y1, y2, ...] のフォーマットになっていること。
 * [x1, x2, ...]は、行列Xへ格納され、[y1, y2, ...]は、行列Yへ格納される。
 * もしファイルがオープンできない場合は、falseを返却する。
 * 
 */
bool loadDataset(char* filename, cv::Mat_<double>& mat) {
	std::ifstream ifs(filename);
    std::string str;
    if (ifs.fail()) {
        return false;
    }
	
	vector<vector<double> > vec;

	getline(ifs, str);
    while (getline(ifs, str)) {
		if (str == "") break;

		vector<double> rec;
		vector<string> list = ml::splitDataset(str, ' ');
		for (int i = 0; i < list.size(); ++i) {
			rec.push_back(stof(list[i]));
		}
		vec.push_back(rec);
	}
	
	mat = cv::Mat_<double>(vec.size(), vec[0].size());

	for (int r = 0; r < vec.size(); ++r) {
		for (int c = 0; c < vec[r].size(); ++c) {
			mat(r, c) = vec[r][c];
		}
	}

	return true;
}

void saveDataset(char* filename, const cv::Mat_<double>& mat) {
	int N = mat.rows;

	ofstream ofs(filename);

	for (int iter = 0; iter < N; ++iter) {
		for (int c = 0; c < mat.cols; ++c) {
			if (c > 0) ofs << " ";
			ofs << mat(iter, c);
		}
		ofs << endl;
	}
	ofs.close();
}

void normalizeDataset(cv::Mat_<double> mat, cv::Mat_<double>& normalized_mat, cv::Mat_<double>& mean, cv::Mat_<double>& stddev) {
	ml::meanStdDev(mat, mean, stddev);

	normalized_mat = (mat - cv::repeat(mean, mat.rows, 1)) / cv::repeat(stddev, mat.rows, 1);
	/*
	// 中央値を計算
	cv::Mat_<double> max_mat, min_mat;
	cv::reduce(mat, max_mat, 0, CV_REDUCE_MAX);
	cv::reduce(mat, min_mat, 0, CV_REDUCE_MIN);
	mu = (max_mat + min_mat) * 0.5;

	// 中央値を0にそろえる
	normalized_mat = mat - cv::repeat(mu, mat.rows, 1);

	// [-1, 1]にする
	cv::reduce(cv::abs(normalized_mat), abs_max, 0, CV_REDUCE_MAX);
	normalized_mat /= cv::repeat(abs_max, mat.rows, 1);
	*/

	/*
	// normalization
	cv::reduce(mat, mu, 0, CV_REDUCE_AVG);
	normalized_mat = mat - cv::repeat(mu, mat.rows, 1);

	// [-1, 1]にする
	cv::reduce(cv::abs(normalized_mat), abs_max, 0, CV_REDUCE_MAX);
	normalized_mat /= cv::repeat(abs_max, mat.rows, 1);
	*/
}

/**
 * 一番右の列に1を追加する。
 */
void addBias(cv::Mat_<double>& data) {
	cv::Mat_<double> tmp = data.clone();
	data = cv::Mat_<double>(tmp.rows, tmp.cols + 1);
	for (int r = 0; r < tmp.rows; ++r) {
		for (int c = 0; c < tmp.cols; ++c) {
			data(r, c) = tmp(r, c);
		}
		data(r, tmp.cols) = 1;
	}
}

cv::Mat mat_square(const cv::Mat& m) {
	return m.mul(m);
}

double mat_sum(const cv::Mat_<double>& m) {
	cv::Mat_<double> s;
	cv::reduce(m, s, 0, CV_REDUCE_SUM);
	cv::reduce(s, s, 1, CV_REDUCE_SUM);
	return s(0, 0);
}

double correlation(const cv::Mat_<double>& m1, const cv::Mat_<double>& m2) {
	// 列ベクトルにする
	cv::Mat_<double> mat1 = m1;
	if (mat1.rows == 1) mat1 = mat1.t();
	cv::Mat_<double> mat2 = m2;
	if (mat2.rows == 2) mat2 = mat2.t();

	// データ数
	int N = mat1.rows;

	// 平均を計算する
	cv::Mat_<double> avg1;
	cv::reduce(mat1, avg1, 0, CV_REDUCE_AVG);
	cv::Mat_<double> avg2;
	cv::reduce(mat2, avg2, 0, CV_REDUCE_AVG);

	cv::Mat_<double> mat1_hat = mat1 - cv::repeat(avg1, N, 1);
	cv::Mat_<double> mat2_hat = mat2 - cv::repeat(avg2, N, 1);

	return mat_sum(mat1_hat.mul(mat2_hat)) / sqrt(mat_sum(mat1_hat.mul(mat1_hat))) / sqrt(mat_sum(mat2_hat.mul(mat2_hat)));
}

/**
 * 平均と標準偏差を計算する。
 * srcの各行が各データ。
 */
void meanStdDev(const cv::Mat_<double>& src, cv::Mat_<double>& mean, cv::Mat_<double>& stddev) {
	int N = src.rows;

	mean = cv::Mat_<double>(1, src.cols);
	stddev = cv::Mat_<double>(1, src.cols);

	for (int c = 0; c < src.cols; ++c) {
		cv::Scalar m, s;
		cv::meanStdDev(src.col(c), m, s);

		mean(0, c) = m.val[0];
		stddev(0, c) = s.val[0] * sqrt((double)N / (N - 1));
	}
}

}