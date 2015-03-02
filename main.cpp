#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;
void clip(Mat& img, float minval, float maxval)
{
	CV_Assert(maxval > minval);
	size_t row = img.rows;
	size_t col = img.cols;
	for (size_t i = 0; i != row; ++i)
	{
		float* temp = img.ptr<float>(i);
		for (size_t j = 0; j != col; ++j)
		{
			if (temp[j] < minval)
			{
				temp[j] = minval;
			}
			if (temp[j] > maxval)
			{
				temp[j] = maxval;
			}
		}
	}
}
void colorTransfer(const Mat& src, Mat& dst)
{
	Mat labsrc, labdst;
	cvtColor(src, labsrc, COLOR_BGR2Lab);
	cvtColor(dst, labdst, COLOR_BGR2Lab);
	labsrc.convertTo(labsrc, CV_32FC3);
	labdst.convertTo(labdst, CV_32FC3);
	//计算三个通道的均值与方差
	Scalar meansrc, stdsrc, meandst, stddst;
	meanStdDev(labsrc, meansrc, stdsrc);
	meanStdDev(labdst, meandst, stddst);
	//三通道分离
	vector<Mat> srcsplit, dstsplit;
	split(labsrc, srcsplit);
	split(labdst, dstsplit);
	//每个通道减去均值
	dstsplit[0] -= meandst[0];
	dstsplit[1] -= meandst[1];
	dstsplit[2] -= meandst[2];
	//每个通道缩放
	dstsplit[0] = stddst[0]/stdsrc[0] * dstsplit[0];
	dstsplit[1] = stddst[1]/stdsrc[0] * dstsplit[1];
	dstsplit[2] = stddst[2]/stdsrc[0] * dstsplit[2];
	//加上源图像的均值
	dstsplit[0] += meansrc[0];
	dstsplit[1] += meansrc[1];
	dstsplit[2] += meansrc[2];
	//控制溢出
	//clip(dstsplit[0], 0.0f, 255.0f);
	//clip(dstsplit[1], 0.0f, 255.0f);
	//clip(dstsplit[2], 0.0f, 255.0f);
	//转换为单字节单通道
	dstsplit[0].convertTo(dstsplit[0], CV_8UC1);
	dstsplit[1].convertTo(dstsplit[1], CV_8UC1);
	dstsplit[2].convertTo(dstsplit[2], CV_8UC1);
	//合并每个通道
	merge(dstsplit, dst);
	//从lab空间转换到RGB空间
	cvtColor(dst, dst, COLOR_Lab2BGR);
}
int main(int argc, char** argv)
{
	Mat src, dst;
	src = imread("D:\\Documents\\Visual Studio 2012\\Projects\\colortransfer\\colortransfer\\images\\ocean_day.jpg", CV_LOAD_IMAGE_COLOR);
	dst = imread("D:\\Documents\\Visual Studio 2012\\Projects\\colortransfer\\colortransfer\\images\\ocean_sunset.jpg", CV_LOAD_IMAGE_COLOR);
	if (src.empty() || dst.empty())
	{
		cerr<<"Error: source image or target image should not be empty!"<<endl;
		return -1;
	}
	if (src.type() != CV_8UC3 || dst.type() != CV_8UC3)
	{
		cerr<<"Error: source image and target image must have RGB channels!"<<endl;
		return -1;
	}
	colorTransfer(src, dst);
	imwrite("dst.jpg", dst);
	imshow("src", src);
	imshow("dst", dst);
	waitKey(0);
	return 0;
}