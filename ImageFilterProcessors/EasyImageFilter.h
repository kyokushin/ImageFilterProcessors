#ifndef EasyImageFilter_h
#define EasyImageFilter_h

#include <opencv/cv.h>

/* TODO: 
 * 2012/12/31 API�̈�����src,dst,���̑��p�����[�^�ɕύX����B
 */

namespace ys {

	//���S�~���Weight�𐶐�����
	cv::Mat genCircleWeight(const cv::Mat& src, int type = 0);
	//��F�̉摜�f�[�^���쐬����B
	cv::Mat genColorImage(const cv::Mat& src, const cv::Scalar& color );

	//weight�̒l�����ɂQ�̉摜���A���t�@�u�����f�B���O����B
	void alphaBlend(
		const cv::Mat& src1,
		const cv::Mat& src2,
		const cv::Mat& weight, //��:src1,��:src2
		cv::Mat& dst
		);

	//�J���[�摜����RGB�l���Ƃ̃O���[�X�P�[���摜���쐬����B
	void split(
		cv::Mat& src,
		cv::Mat& dst_r,
		cv::Mat& dst_g,
		cv::Mat& dst_b
		);

	//3���̃O���[�X�P�[���摜����1���̃J���[�摜���쐬����B
	void merge(
		cv::Mat& src_r,
		cv::Mat& src_g,
		cv::Mat& src_b,
		cv::Mat& dst
		);

	//�F�ɂ��N���X�^�����O���s���A�N���X�^���Ƃ̉摜��Ԃ�
	void clustering(
		cv::Mat& src,
		std::vector<cv::Mat> dsts,
		int cluster_num = 5
		);

	//1�����͕������̉摜���Ȃ���1���̉摜�ɂ���B
	void connect(
		std::vector<cv::Mat> srcs,
		cv::Mat dst,
		int rows = 2,
		int cols = 2
		);

	//�摜�̊g��k��
	void resize(
		cv::Mat src,
		cv::Size size,
		cv::Mat dst
		);

	//�摜����ꕔ�؂�o��
	void clip(
		cv::Mat& src,
		cv::Rect& rect,
		cv::Mat& dst
		);

	//�F�̔��]������B
	//weight�Ȃ甒�����]
	//�J���[�摜�Ȃ�l�K�E�|�W���]
	void invert(
		cv::Mat& src,
		cv::Mat& dst
		);




	/*�ȉ��͎����ς݁B*/

	

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