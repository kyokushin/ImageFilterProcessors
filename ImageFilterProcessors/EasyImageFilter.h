#ifndef EasyImageFilter_h
#define EasyImageFilter_h

#include <opencv/cv.h>

/* TODO: 
 * 2012/12/31 APIの引数をsrc,dst,その他パラメータに変更する。
 */

namespace ys {

	//同心円上のWeightを生成する
	cv::Mat genCircleWeight(const cv::Mat& src, int type = 0);
	//一色の画像データを作成する。
	cv::Mat genColorImage(const cv::Mat& src, const cv::Scalar& color );

	//weightの値を元に２つの画像をアルファブレンディングする。
	void alphaBlend(
		const cv::Mat& src1,
		const cv::Mat& src2,
		const cv::Mat& weight, //白:src1,黒:src2
		cv::Mat& dst
		);

	//カラー画像からRGB値ごとのグレースケール画像を作成する。
	void split(
		cv::Mat& src,
		cv::Mat& dst_r,
		cv::Mat& dst_g,
		cv::Mat& dst_b
		);

	//3枚のグレースケール画像から1枚のカラー画像を作成する。
	void merge(
		cv::Mat& src_r,
		cv::Mat& src_g,
		cv::Mat& src_b,
		cv::Mat& dst
		);

	//色によるクラスタリングを行い、クラスタごとの画像を返す
	void clustering(
		cv::Mat& src,
		std::vector<cv::Mat> dsts,
		int cluster_num = 5
		);

	//1枚又は複数枚の画像をつなげて1枚の画像にする。
	void connect(
		std::vector<cv::Mat> srcs,
		cv::Mat dst,
		int rows = 2,
		int cols = 2
		);

	//画像の拡大縮小
	void resize(
		cv::Mat src,
		cv::Size size,
		cv::Mat dst
		);

	//画像から一部切り出し
	void clip(
		cv::Mat& src,
		cv::Rect& rect,
		cv::Mat& dst
		);

	//色の反転をする。
	//weightなら白黒反転
	//カラー画像ならネガ・ポジ反転
	void invert(
		cv::Mat& src,
		cv::Mat& dst
		);




	/*以下は実装済み。*/

	

	void weightedSaturation(
		cv::Mat& src,
		cv::Mat &dst,
		cv::Mat& weight,
		double scale
		);

	void weightedGaussian(
		cv::Mat& src,
		cv::Mat& dst,
		cv::Mat& weight,
		double scale
		);

	void genLineWeight(
		const cv::Size& size,
		const cv::Point& start_pos,
		const cv::Point& end_pos,
		cv::Mat& weight
		);

	void genObjectRegionWeight(
		const cv::Mat& src,
		const cv::Point& lt_pos,
		const cv::Point& rb_pos,
		cv::Mat& weight,
		int iter_count = 4
		);


};

#endif