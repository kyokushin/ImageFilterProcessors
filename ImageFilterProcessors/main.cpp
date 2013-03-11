#include <opencv/cv.h>
#include <opencv/highgui.h>


#include <string>
#include <iostream>
#include <map>
#include <vector>

#ifdef _WIN32
#include "win_lib.h"
#include "opencv_windows_lib.h"
#else
#include "qapplication.h"
#include "qt_lib.h"
#endif

#include "EasyImageFilter.h"
#include "NoiseReduction.h"

using namespace std;
using namespace cv;

/* TODO:
* 2012/12/31 doubleで計算しなくてもいいところはintに変更
* weight 用 Mat も単位を10000とかに設定して値を保持できるように拡張してもいいかも
*/

struct UIDataContainer {
	Mat *pSrc_img;
	Mat *pShow_img;
	bool line_pos_available;
	Point start_pos;
	Point end_pos;
	Point current_pos;
	Scalar line_color;
	Scalar mask_color;

	string *pWname;

	UIDataContainer()
		:line_color(255,0,0), mask_color(0,0,255)
	{}

};

void mouse_rect( int event, int x, int y, int flags, void *userdata )
{
	UIDataContainer& uiData = *((UIDataContainer*)userdata);
	Mat &src_img = *uiData.pSrc_img;
	Mat &show_img = *uiData.pShow_img;
	Scalar &line_color = uiData.line_color;
	Scalar &mask_color = uiData.mask_color;
	string &wname = *uiData.pWname;

	bool update_window = false;


	if( event == CV_EVENT_MOUSEMOVE 
		&& ( flags & CV_EVENT_FLAG_LBUTTON ) )
	{
		Point &pos = uiData.current_pos;
		pos.x = x;
		pos.y = y;

		src_img.copyTo( show_img );
		rectangle( show_img, uiData.start_pos, pos, line_color, 3 );

		update_window = true;
	}
	else if( event == CV_EVENT_LBUTTONDOWN ){
		uiData.line_pos_available = false;
		Point& pos = uiData.start_pos;
		pos.x = x;
		pos.y = y;

		src_img.copyTo( show_img );
		circle( show_img, pos, 3, line_color );

		update_window = true;
	}
	else if( event == CV_EVENT_LBUTTONUP ){
		Point& pos = uiData.end_pos;
		pos.x = x;
		pos.y = y;

		src_img.copyTo( show_img );
		rectangle( show_img, uiData.start_pos, pos, line_color, 3 );

		uiData.line_pos_available = true;
		update_window = true;
	}

	if( update_window ){
		imshow( *(uiData.pWname), show_img );
		waitKey(10);
	}

}

void mouse_stroke( int event, int x, int y, int flags, void *userdata )
{
	UIDataContainer& uiData = *((UIDataContainer*)userdata);
	Mat &src_img = *uiData.pSrc_img;
	Mat &show_img = *uiData.pShow_img;
	Scalar &line_color = uiData.line_color;
	Scalar &mask_color = uiData.mask_color;
	string &wname = *uiData.pWname;

	bool update_window = false;


	if( event == CV_EVENT_MOUSEMOVE 
		&& ( flags & CV_EVENT_FLAG_LBUTTON ) )
	{
		Point &pos = uiData.current_pos;
		pos.x = x;
		pos.y = y;

		src_img.copyTo( show_img );
		line( show_img, uiData.start_pos, pos, line_color, 3 );

		update_window = true;
	}
	else if( event == CV_EVENT_LBUTTONDOWN ){
		uiData.line_pos_available = false;
		Point& pos = uiData.start_pos;
		pos.x = x;
		pos.y = y;

		src_img.copyTo( show_img );
		circle( show_img, pos, 3, line_color );

		update_window = true;
	}
	else if( event == CV_EVENT_LBUTTONUP ){
		Point& pos = uiData.end_pos;
		pos.x = x;
		pos.y = y;

		src_img.copyTo( show_img );
		line( show_img, uiData.start_pos, pos, line_color, 3 );

		uiData.line_pos_available = true;
		update_window = true;
	}

	if( update_window ){
		imshow( *(uiData.pWname), show_img );
		waitKey(10);
	}

}

void showHelp(){
	cout<< "+++++ USAGE +++++" <<endl
		<< "'-' : resize to 1/2" <<endl
		<< "'d' : distance to line weight" <<endl
		<< "'g' : grabcut weight" <<endl
		<< "'t' : weighted gaussian" <<endl
		<< "'s' : weighted saturation" <<endl
		<< "'i' : calc inverse weight" <<endl
		<< "'l' : change to line selector" <<endl
		<< "'r' : change to rectangle selector" <<endl
		<< "'c' : generate circle weight" <<endl
		<< "'a' : alpha blending (use another image)" <<endl
		<< "'h' : show help" <<endl
		<<endl;
}

int main( int argc, char** argv )
{	
	QApplication app(argc, argv);

	string fname;
	bool selected = fileSelectDialog( fname );
	//bool selected = false;
	if( !selected ){
		cerr<< "not selected file" <<endl;
		return -1;
	}

	Mat src_img = imread( fname );
	Mat show_img = src_img.clone();
	Mat dst_img;
	string wname( "ImageFilter" );

	UIDataContainer uiData;
	uiData.pSrc_img = &src_img;
	uiData.pShow_img = &show_img;
	uiData.pWname = &wname;

	namedWindow( wname );
	setMouseCallback( wname, mouse_stroke, (void*)&uiData );

	Mat distance_mat( src_img.size(), CV_64F );

	showHelp();

	int ikey = -1;
	while( ikey != 0x1b )
	{
		imshow( wname, show_img );
		ikey = waitKey();
		char ckey = (char)ikey;
		if( ckey == '-' )
		{
			resize( src_img, src_img, Size( src_img.cols/2, src_img.rows/2));
			distance_mat.create( src_img.size(), CV_64F);
			src_img.copyTo( show_img );
		}
		if( ckey == 'd' )
		{
			cout<< "generate => distance to line weight" <<endl;

			ys::genLineWeight(
				distance_mat.size(),
				uiData.start_pos,
				uiData.end_pos,
				distance_mat
				);

			imshow( "weight", distance_mat );
			waitKey(10);

			/*
			Mat mask_mat;
			distance_mat = distance_mat * 255.0;
			distance_mat.convertTo( mask_mat, CV_8U );

			Mat mat_zero = Mat::zeros(mask_mat.size(), mask_mat.type());
			Mat mat_arr[] = { mat_zero, mat_zero, mask_mat };

			merge( mat_arr, 3, mask_mat );

			show_img = 0.5 * src_img + 0.5 * mask_mat;
			*/

		}
		else if( ckey == 'g' ){
			cout<< "start grab cut" <<endl;
			ys::genObjectRegionWeight(
				src_img,
				uiData.start_pos,
				uiData.end_pos,
				distance_mat 
				);

			cout<< "end grab cut" <<endl
				<< "show weight image" <<endl;

			GaussianBlur( distance_mat, distance_mat, Size(5,5), 4 );

			imshow( "weight", distance_mat );
			waitKey(10);


		}
		else if( ckey == 't' ){
			cout<< "calc weighted gaussian image" <<endl;

			ys::weightedGaussian( src_img, dst_img, distance_mat, 30);
			show_img = dst_img;
		}
		else if( ckey == 's' ){
			cout<< "calc weighted saturation image" <<endl;

			ys::weightedSaturation( src_img, dst_img, distance_mat, 1.0 );
			show_img = dst_img;
		}
		else if( ckey == 'l' ){
			cout<< "change to line selector" <<endl;
			setMouseCallback( wname, mouse_stroke, (void*)&uiData );
		}
		else if( ckey == 'r' ){
			cout<< "change to rectangle selector" <<endl;
			setMouseCallback( wname, mouse_rect, (void*)&uiData );
		}
		else if( ckey == 'i' ){
			cout<< "calc inverse weight" <<endl;
			distance_mat = 1 - distance_mat;
			imshow( "weight", distance_mat );
			waitKey(10);
		}
		else if( ckey == 'n' ){
			cout<< "noise reduction" <<endl;
			//bilateralFilter( src_img, dst_img, 5, 200.0, 50.0 );
			//ys::noiseReduction(src_img, dst_img, 10 );
			cv::Mat mat_array[3];
			cv::split( src_img, mat_array );
			cout<< "channels:" << src_img.channels() <<endl;

			cv::Mat r_dst, g_dst, b_dst;
			ys::rowNoisereduction( mat_array[0], r_dst, 50, 20);
			ys::rowNoisereduction( mat_array[1], g_dst, 50, 20);
			ys::rowNoisereduction( mat_array[2], b_dst, 50, 20);
			cv::Mat dst_array[3] = {r_dst, g_dst, b_dst};
			cv::merge( dst_array, 3, dst_img );
			cv::imshow( "orig", src_img );
			show_img = dst_img;
		}
		else if( ckey == 'c' ){
			cout<< "circle weight" <<endl;
			distance_mat = ys::genCircleWeight( src_img, 1 );
			imshow( "weight", distance_mat );
			cv::waitKey(10);
		}
		else if( ckey == 'a' ){
			cout<< "alpha blending" <<endl;
			string bg_fname;
			bool selected = fileSelectDialog( bg_fname );
			Mat bg_image;
			if( !selected ){
				cout<< "select color" <<endl;

				cv::Scalar color;
				colorPicker( color );

				bg_image = ys::genColorImage( src_img, color );
			}
			else {
				bg_image = imread( bg_fname );
			}
			ys::alphaBlend( src_img, bg_image, distance_mat, dst_img );
			show_img = dst_img;
		}
		else if( ckey == 'm' ){
			cv::Mat black = ys::genColorImage( src_img, Scalar(0,0,0));
			cv::Mat brown = ys::genColorImage( src_img, Scalar(0xb3,0xde,0xf5));
			cv::Mat filter;
			cv::Mat circle_weight = ys::genCircleWeight( src_img, 1);
			ys::alphaBlend( black, brown, circle_weight, filter );

			/*
			cv::Mat uniform_weight(src_img.rows, src_img.cols,CV_64FC1 );
			uniform_weight.setTo( 0.75 );
			ys::alphaBlend( src_img, filter, uniform_weight, dst_img );
			*/
			circle_weight = ys::genCircleWeight( src_img, 1 );
			ys::invert( circle_weight, circle_weight);
			ys::alphaBlend( src_img, filter, circle_weight, dst_img );

			show_img = dst_img;
		}
		else if( ckey == 'S' ){
			cout<< "save" <<endl;
			imwrite("dst.png", dst_img);
		}
		else if( ckey == 'h' ){
			showHelp();
		}
	}
}
