#include <cmath>

#include <opencv/cv.h>
#include "NoiseReduction.h"

using namespace std;

double membershipFunction( int u, int K )
{
	int abs_u = abs(u);
	if( 0<= abs_u && abs_u <= K ) return 1 - abs_u/(double)K;
	return 0;
}

void lambda( int u, int L, double* positive, double* negative ){

	//Positive
	if( u > L ) *positive =  1.0;
	else if( u < 0 ) *positive = 0.0;
	else *positive = u/(double)L;

	//Negative
	int mL = -L;
	if( u < L ) *negative = 1.0;
	else if( u > 0 ) *negative = 0.0;
	else *negative = u/(double)mL;
}

uchar fuzzyFilter(const cv::Mat& src, int x, int y, int K)
{
	//今のところ1ch画像（グレースケール）のみに対応

	//注目画素x,yからの相対的な画素の位置
	static const int dir_pos[8][2] = {
		{-1,-1}, //NW
		{-1, 0}, //W
		{-1, 1}, //SW
		{ 0, 1}, //S
		{ 1, 1}, //SE
		{ 1, 0}, //E
		{ 1,-1}, //NE
		{ 0,-1}  //N
	};

	static const int directions[8][3][2] = {
		{{-1, 1}, {0,0}, { 1,-1}},//NW
		{{ 0, 1}, {0,0}, { 0,-1}},//W
		{{ 1, 1}, {0,0}, {-1,-1}},//SW
		{{ 1, 0}, {0,0}, {-1, 0}},//S
		{{ 1,-1}, {0,0}, {-1, 1}},//SE
		{{ 0,-1}, {0,0}, { 0, 1}},//E
		{{-1,-1}, {0,0}, { 1, 1}},//NE
		{{-1, 0}, {0,0}, { 1, 0}} //N
	};
	static const int direction_num = 8;

	double mf_val[3]; //各ディレクションでのメンバーシップ関数の値

	double fd_val[8]; //Fuzzy derivative の値
	double lambda_val[8][2]; //lambda+と-の値

	uchar* src_data = src.data;
	int src_wstep = src.step;
	int channel = src.channels();

	if( channel != 1 ) return 0;

	double sum_of_lambda = 0.0;

	for( int dir_i=0; dir_i<direction_num; dir_i++ ){
		for( int i=0; i<3; i++ ){
			int c_y = y + directions[dir_i][i][1];
			int c_x = x + directions[dir_i][i][0];

			int d_y = c_y + dir_pos[dir_i][1];
			int d_x = c_x + dir_pos[dir_i][0];

			int d = src_data[ d_y * src_wstep + d_x ] - src_data[c_y * src_wstep + c_x];
			mf_val[i] = membershipFunction( d, K );
		}

		fd_val[dir_i] = max(
			max(
			min( mf_val[1], mf_val[0] ),
			min( mf_val[1], mf_val[2] )
			),
			min( mf_val[0], mf_val[2] )
			);

		double p,n;
		int c_y = y + directions[dir_i][1][1];
		int c_x = x + directions[dir_i][1][0];

		int d_y = c_y + dir_pos[dir_i][1];
		int d_x = c_x + dir_pos[dir_i][0];

		int d = src_data[ d_y * src_wstep + d_x ] - src_data[c_y * src_wstep + c_x];
		lambda( d, 255, &p, &n );

		lambda_val[dir_i][0] = min( fd_val[dir_i], p);
		lambda_val[dir_i][1] = min( fd_val[dir_i], n);

		sum_of_lambda = lambda_val[dir_i][0] - lambda_val[dir_i][1];
	}



	double ret_val = src_data[y*src_wstep + x] * sum_of_lambda / 8; 
	if( ret_val > 255 ){
		ret_val = 255;
	}

	return (uchar)ret_val;
}

void ys::noiseReduction(
	cv::Mat& src,
	cv::Mat& dst,
	int K)
{
	cv::Mat src_gray;
	cvtColor( src, src_gray, CV_BGR2GRAY );
	dst = src_gray.clone();

	int width = dst.cols;
	int height = dst.rows;
	int channel = dst.channels();

	for( int i=2; i<height-2; i++ ){
		uchar* dst_line = dst.ptr(i);
		for( int h=2; h<width-2; h++ ){

			dst_line[h] = fuzzyFilter( src_gray, h, i, K);

		}
	}

}
double edgeLevel(const cv::Mat& src, int x, int y)
{
	//今のところ1ch画像（グレースケール）のみに対応

	//注目画素x,yからの相対的な画素の位置
	static const int dir_pos[8][2] = {
		{-1,-1}, //NW
		{-1, 0}, //W
		{-1, 1}, //SW
		{ 0, 1}, //S
		{ 1, 1}, //SE
		{ 1, 0}, //E
		{ 1,-1}, //NE
		{ 0,-1}  //N
	};

	static const int directions[8][3][2] = {
		{{-1, 1}, {0,0}, { 1,-1}},//NW
		{{ 0, 1}, {0,0}, { 0,-1}},//W
		{{ 1, 1}, {0,0}, {-1,-1}},//SW
		{{ 1, 0}, {0,0}, {-1, 0}},//S
		{{ 1,-1}, {0,0}, {-1, 1}},//SE
		{{ 0,-1}, {0,0}, { 0, 1}},//E
		{{-1,-1}, {0,0}, { 1, 1}},//NE
		{{-1, 0}, {0,0}, { 1, 0}} //N
	};
	static const int direction_num = 8;

	double mf_val[3]; //各ディレクションでのメンバーシップ関数の値

	double fd_val[8]; //Fuzzy derivative の値
	double lambda_val[8][2]; //lambda+と-の値

	uchar* src_data = src.data;
	int src_wstep = src.step;
	int channel = src.channels();

	if( channel != 1 ){
		cout<< "channel:" << channel <<endl;
		return 0;
	}
	double sum_of_lambda = 0.0;
	//cout<< "in edgeLevel" <<endl;

	for( int dir_i=0; dir_i<direction_num; dir_i++ ){
		for( int i=0; i<3; i++ ){
			int c_y = y + directions[dir_i][i][1];
			int c_x = x + directions[dir_i][i][0];

			int d_y = c_y + dir_pos[dir_i][1];
			int d_x = c_x + dir_pos[dir_i][0];

			int d = src_data[ d_y * src_wstep + d_x ] - src_data[c_y * src_wstep + c_x];
			mf_val[i] = abs(d);
			//cout<< "mf_val:" << mf_val[i] <<endl; 
		}

		fd_val[dir_i] = max(
			max(
			min( mf_val[1], mf_val[0] ),
			min( mf_val[1], mf_val[2] )
			),
			min( mf_val[0], mf_val[2] )
			);

		sum_of_lambda += fd_val[dir_i];
	}

	return sum_of_lambda; 

}

static const int max_ksize = 21;//奇数

int median(const cv::Mat& src, int x, int y, int ksize, int c_th )
{
	int vals[max_ksize*max_ksize];

	int end_x = ksize/2;
	int start_x = -end_x;
	int end_y = end_x;
	int start_y = start_x;

	if( x + end_x >= src.cols ) end_x = src.cols - x-1;
	if( x + start_x < 0 ) start_x = -x;
	if( y + end_y >= src.rows ) end_y = src.rows - y-1;
	if( y + start_y < 0 ) start_y = -y;
	
	//if( ksize == 7 ) getchar();

	int x_size = end_x - start_x + 1;
	int y_size = end_y - start_y + 1;

	assert( x_size <= max_ksize );

	int counter = 0;
	for( int i=start_y; i<=end_y; i++ ){
		const uchar* src_line = src.ptr(y+i);
		for( int h=start_x; h<=end_x; h++ ){
			int sub = abs((int)src_line[h] - src.ptr(y)[x]);
			//if( sub > c_th ) continue;

			vals[counter++] = src_line[x+h];
		}
	}
	sort(vals,vals+counter);
	return vals[counter/2];
}

void ys::rowNoisereduction(
	cv::Mat& src,
	cv::Mat& dst,
	int K, int c_th)
{
	cout<< "start row noise reduction" <<endl;

	int height = src.rows;
	int width = src.cols;

	dst.create( height, width, CV_8UC1 );

	for( int i=2; i<height-2; i++){
		uchar* dst_line = dst.ptr(i);
		for( int h=2; h<width-2; h++ ){
			double val = edgeLevel( src, h, i);

			//cout<< val <<endl; 

			if( val < K ) val = 1 - val /K;
			else val = 0;

			//dst_line[h] = 255*val;

			int ksize = val * max_ksize;
			if( ksize%2 == 0 ) ksize++;
			if( ksize <= 1 ) dst_line[h] = src.ptr(i)[h];
			else {
				dst_line[h] = median( src, h,i, ksize, c_th );
			}
		}
	}

}