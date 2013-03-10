#include "EasyImageFilter.h"

#include <opencv/cv.h>

using namespace std;
using namespace cv;


//同心円上のWeightを生成する
cv::Mat ys::genCircleWeight(const cv::Mat& src, int type)
{
	assert( src.type() == CV_8UC3 );

	int width = src.cols;
	int height = src.rows;
	cv::Mat weight(height, width, CV_64FC1);

	cv::Point center_pos( width/2, height/2 );

	if( type == 0 ){

		double max_length = sqrt(center_pos.ddot(center_pos));

		for( int i=0; i<height; i++ ){
			double* weight_line = (double*)weight.ptr(i);

			for( int h=0; h<width; h++ ){

				cv::Point sub_pos(h-center_pos.x, i-center_pos.y);

				weight_line[h] = sqrt(sub_pos.ddot(sub_pos))/max_length;

			}
		}
	}
	else if( type == 1 ){
		double max_length2 = (double)center_pos.dot(center_pos);

		for( int i=0; i<height; i++ ){
			double* weight_line = (double*)weight.ptr(i);

			for( int h=0; h<width; h++ ){

				cv::Point sub_pos(h-center_pos.x, i-center_pos.y);

				weight_line[h] = sub_pos.ddot(sub_pos)/max_length2;

			}
		}
	}

	return weight;
}


//一色の画像データを作成する。
cv::Mat ys::genColorImage(const cv::Mat& src, const cv::Scalar& color )
{
	cv::Mat bg_image( src.rows, src.cols, CV_8UC3 );
	
	bg_image.setTo(color);
	return bg_image;
}


//weightの値を元に２つの画像をアルファブレンディングする。
//白いほどsrc1の割合が高い
void ys::alphaBlend(
	const cv::Mat& src1,
	const cv::Mat& src2,
	const cv::Mat& weight, //白:src1,黒:src2
	cv::Mat& dst
	)
{
	assert( src1.type() == CV_8UC3 );
	assert( src2.type() == CV_8UC3 );
	assert( weight.type() == CV_64FC1 );

	int width = src1.cols;
	int height = src1.rows;

	if( width > src2.cols ) width = src2.cols;
	if( height > src2.rows ) height = src2.rows;
	if( width > weight.cols ) width = weight.cols;
	if( height > weight.rows ) height = weight.rows;

	dst.create( height, width, CV_8UC3);

	for( int i=0; i<height; i++ ){
		const uchar* src1_line = src1.ptr(i);
		const uchar* src2_line = src2.ptr(i);
		const double* weight_line = (double*)weight.ptr(i);
		uchar* dst_line = dst.ptr(i);

		for( int h=0; h<width; h++ ){
			const uchar* src1_pix = src1_line + 3*h;
			const uchar* src2_pix = src2_line + 3*h;
			double weight_val = weight_line[h];
			uchar* dst_pix = dst_line +3*h;

			assert( weight_val <= 1.0 );

			for( int c=0; c<3; c++ ){
				double pix_val = src1_pix[c] * weight_val + src2_pix[c] * (1-weight_val);

				dst_pix[c] = (uchar)pix_val;
			}
		}
	}


}

//カラー画像からRGB値ごとのグレースケール画像を作成する。
void ys::split(
	cv::Mat& src,
	cv::Mat& dst_r,
	cv::Mat& dst_g,
	cv::Mat& dst_b
	){}

//3枚のグレースケール画像から1枚のカラー画像を作成する。
void ys::merge(
	cv::Mat& src_r,
	cv::Mat& src_g,
	cv::Mat& src_b,
	cv::Mat& dst
	){}

//色によるクラスタリングを行い、クラスタごとの画像を返す
void ys::clustering(
	cv::Mat& src,
	std::vector<cv::Mat> dsts,
	int cluster_num
	){}

//1枚又は複数枚の画像をつなげて1枚の画像にする。
void ys::connect(
	std::vector<cv::Mat> srcs,
	cv::Mat dst,
	int rows,
	int cols
	){}

//画像の拡大縮小
void ys::resize(
	cv::Mat src,
	cv::Size size,
	cv::Mat dst
	){}

//画像から一部切り出し
void ys::clip(
	cv::Mat& src,
	cv::Rect& rect,
	cv::Mat& dst
	){}

//色の反転をする。
//weightなら白黒反転
//カラー画像ならネガ・ポジ反転
void ys::invert(
	cv::Mat& src,
	cv::Mat& dst
	)
{

	if( src.type() == CV_64FC1 ){
		dst = 1 - src;
	}



}


/*以下は実装済み。*/

void ys::weightedSaturation(
	cv::Mat& src,
	cv::Mat &dst,
	cv::Mat& weight,
	double scale
	)
{


	if( weight.type() != CV_64F ){
		cerr<< "weight type is not CV_64F" <<endl;
		return;
	}
	if( src.type() != CV_8UC3 ){
		cerr<< "weight type is not CV_64F" <<endl;
		return;
	}
	if( src.size() != weight.size() ){
		Size &src_size = src.size();
		Size &weight_size = weight.size();
		cerr<< "not equal src size and weight size" <<endl
			<< "src : " << src_size.width << "x" << src_size.height <<endl
			<< "weight : " << weight_size.width << "x" << weight_size.height <<endl;
	}

	dst.create( src.size(), src.type() );
	int rows = dst.rows;
	int cols = dst.cols;

	Mat hsv_img;
	cvtColor( src, hsv_img, CV_BGR2HSV );
	for( int i=0; i<rows; i++ ){
		unsigned char* hsv_data = (unsigned char*)hsv_img.ptr(i);
		double* weight_data = (double*)weight.ptr(i);

		for( int h=0; h<cols; h++ ){
			unsigned char* hsv_pix = hsv_data + 3 * h ;
			hsv_pix[1] *= weight_data[h];
		}
	}

	cvtColor( hsv_img, dst, CV_HSV2BGR );
}

void ys::weightedGaussian(
	cv::Mat& src,
	cv::Mat& dst,
	cv::Mat& weight,
	double scale
	)
{

	if( weight.type() != CV_64F ){
		cerr<< "weight type is not CV_64F" <<endl;
		return;
	}
	if( src.type() != CV_8UC3 ){
		cerr<< "weight type is not CV_64F" <<endl;
		return;
	}
	if( src.size() != weight.size() ){
		Size &src_size = src.size();
		Size &weight_size = weight.size();
		cerr<< "not equal src size and weight size" <<endl
			<< "src : " << src_size.width << "x" << src_size.height <<endl
			<< "weight : " << weight_size.width << "x" << weight_size.height <<endl;
	}

	dst.create( src.size(), src.type() );

	int rows = src.rows;
	int cols = src.cols;

	map<int,vector<double> > weight_gauss_map;
	const int gauss_size = 7;
	const int gauss_center= gauss_size/2;

	for( int i=0; i<rows; i++ ){
		unsigned char* dst_data = (unsigned char*)dst.ptr(i);
		double* weight_data = (double*)weight.ptr(i);

		//cout<< "current row : " << i <<endl;

		for( int h=0; h<cols; h++ ){

			double weight = weight_data[h];
			vector<double> &gauss = weight_gauss_map[(int)(weight*1000)];//計算の高速化のためのルックアップテーブル

			//ガウス分布の生成
			if( gauss.size() == 0 ){
				gauss.reserve(gauss_size * gauss_size);
				double total = 0.0;
				for( int y=0; y<gauss_size; y++){
					for( int x=0; x<gauss_size; x++ ){
						const int cx = x - gauss_center;
						const int cy = y - gauss_center;
						const int dist = cx * cx + cy * cy ;
						double tmp_val = exp( -(double)( dist )/(2.0 * (weight + 0.0001 ) * scale) );
						total += tmp_val;
						gauss.push_back( tmp_val );
					}
				}

				if( gauss.size() != gauss_size * gauss_size ){
					cerr<< "!!! failed to not match gauss mask size !!!" <<endl;
					return;
				}

				for( int g_i=0; g_i<gauss_size*gauss_size; g_i++ ){
					gauss[g_i] = gauss[g_i]/total;
				}

				/*
				for( int g_i=0; g_i<gauss_size*gauss_size; g_i++ ){
				cout<< "[" << g_i << "]" <<  gauss[g_i]; 
				}
				cout<<endl;
				*/
				//cout<< "gauss weight num : " << weight_gauss_map.size() <<endl;
			}

			double dst_r=0, dst_g=0, dst_b=0;
			for( int y=0; y<gauss_size; y++ ){

				int y_i = i + y - gauss_center;
				if( y_i < 0 ) y_i *= -1;
				else if( rows <= y_i ) y_i = 2*rows - y_i - 1;
				unsigned char* src_data = (unsigned char*)src.ptr(y_i);

				for( int x=0; x<gauss_size; x++ ){

					int x_h = h + x - gauss_center;
					if( x_h < 0 ) x_h *= -1;
					else if( cols <= x_h ) x_h = 2*cols - x_h;


					unsigned char* src_pix = src_data + 3*x_h;
					double gauss_val = gauss[ gauss_size * y + x ];
					dst_r += src_pix[2] * gauss_val;
					dst_g += src_pix[1] * gauss_val;
					dst_b += src_pix[0] * gauss_val;

				}
			}

			unsigned char* dst_pix = dst_data + 3*h;
			dst_pix[2] = (unsigned char)dst_r;
			dst_pix[1] = (unsigned char)dst_g;
			dst_pix[0] = (unsigned char)dst_b;
		}
	}
}

void ys::genLineWeight(
	const cv::Size& size,
	const cv::Point& start_pos,
	const cv::Point& end_pos,
	cv::Mat& weight
	)
{
	weight.create( size, CV_64FC1 );

	double a = end_pos.y - start_pos.y;
	double b = start_pos.x - end_pos.x;
	double c = - start_pos.y * b - start_pos.x * a;

	cout<< "a:" << a <<endl;
	cout<< "b:" << b <<endl;
	cout<< "c:" << c <<endl;

	double den = sqrt( a * a + b * b );

	int rows = size.height;
	int cols = size.width;

	double max_distance = 0;
	Point len_pos = end_pos - start_pos;
	double line_length =   sqrt( (double)(len_pos.dot( len_pos ))/(rows * rows + cols * cols ));

	for( int i=0; i<rows; i++ ){
		double* data = (double*)weight.ptr(i);

		for( int h=0; h<cols; h++ ){
			double line_dist = abs(a * h + b * i + c);
			Point sub_pos( start_pos.x-h,start_pos.y-i);
			double circle_dist = sqrt((double)sub_pos.dot( sub_pos ));
			//cout<< circle_dist <<endl;
			//line_dist *= line_dist;
			double dist = line_dist * circle_dist * line_length;
			data[h] = dist;

			if( max_distance < dist ){
				max_distance = dist;
			}
		}
	}
	//cout<< distance_mat <<endl;
	cout<< start_pos <<endl;
	cout<< end_pos <<endl;
	weight = (weight + max_distance * 0.01 )/ max_distance;
}

void ys::genObjectRegionWeight(
	const cv::Mat& src,
	const cv::Point& start_pos,
	const cv::Point& end_pos,
	cv::Mat& weight,
	int iter_count
	){

		Mat mask_img, bgdModel, fgdModel, small_src;

		resize( src, small_src, Size( src.cols/2, src.rows/2), 1, 1, INTER_LANCZOS4 );
		Point small_start_pos( start_pos.x/2, start_pos.y/2);
		Point small_end_pos( end_pos.x/2, end_pos.y/2 );
		grabCut( small_src, mask_img, Rect( small_start_pos, small_end_pos ), bgdModel, fgdModel, 4, GC_INIT_WITH_RECT );
		Mat big_mask;
		resize( mask_img, big_mask, src.size(), 1,1, INTER_LANCZOS4);

		weight.create(src.size(), CV_64FC1 );
		double* distance_data = (double*)weight.data;
		unsigned char* mask_data = (unsigned char*)big_mask.data;
		for( int i=0; i< src.rows * src.cols; i++ ){
			switch( mask_data[i] ){
			case GC_BGD:
				distance_data[i] = 0;
				break;
			case GC_FGD:
				distance_data[i] = 1;
				break;
			case GC_PR_BGD:
				distance_data[i] = 0;
				break;
			case GC_PR_FGD:
				distance_data[i] = 1;
				break;

			}
		}
}

