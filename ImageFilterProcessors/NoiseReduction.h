#ifndef NoiseReduction_h
#define NoiseReduction_h

namespace ys {
	void noiseReduction(
		cv::Mat& src,
		cv::Mat& dst,
		int K);
	void rowNoisereduction(
		cv::Mat& src,
		cv::Mat& dst,
		int K, int c_th);

};

#endif