
#include <iostream>
#include <vector>
#include <stdio.h>
#include <stdarg.h>
#include "fstream"
#include "iostream"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <time.h>
using namespace std;
using namespace cv;


int main(int argc, char ** argv)
{
	cv::Mat img;
	cv::Mat raw;
		img = cv::imread("D.png", 0);
	cv::threshold(img, img, 50, 255, cv::THRESH_BINARY_INV);
	cv::bitwise_not(img, img);
	img.copyTo(raw);
	cv::resize(raw, raw, cv::Size(300, 300), CV_INTER_NN);
	cv::imshow("RAW", raw);
	cv::Mat skel(img.size(), CV_8UC1, cv::Scalar(0));
	cv::Mat temp;
	cv::Mat eroded;

	cv::Mat element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(3, 3));

	bool done;
	do
	{

		cv::erode(img, eroded, element);
		cv::dilate(eroded, temp, element); // temp = open(img)
		cv::subtract(img, temp, temp);
		cv::bitwise_or(skel, temp, skel);
		eroded.copyTo(img);

		done = (cv::countNonZero(img) == 0);
	} while (!done);
	Mat structuringElement = cv::getStructuringElement(CV_SHAPE_ELLIPSE, Size(10, 10), cv::Point(-1, -1));
	cv::morphologyEx(skel, skel, 3, structuringElement);
	vector<Vec4i> hierarchy;
	vector<vector<Point> > contours;
	findContours(skel, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	vector<Point2f> corners;
	double qualityLevel = 0.01;
	double minDistance = 10;
	int blockSize = 3;
	bool useHarrisDetector = false;
	double k = 0.04;
	int maxCorners = 23;

	Point pCenter;
	Mat	 drawing = Mat::zeros(skel.size(), CV_8UC1);
	//	drawing = skel.clone();
	for (size_t i = 0; i< contours.size(); i++)
	{
		Scalar color = Scalar(255, 255, 255);
		double area = contourArea(contours[i]);
		if (area > 30)
		{
			drawContours(drawing, contours, (int)i, color, 1, LINE_8, hierarchy, 0);
			int sz = static_cast<int>(contours[i].size());
			Mat data_pts = Mat(sz, 2, CV_64FC1);
			for (int j = 0; j < data_pts.rows; ++j)
			{
				data_pts.at<double>(j, 0) = contours[i][j].x;
				data_pts.at<double>(j, 1) = contours[i][j].y;
			}
			PCA pca_analysis(data_pts, Mat(), CV_PCA_DATA_AS_ROW);
			pCenter = Point(pca_analysis.mean.at<double>(0, 0),
				pca_analysis.mean.at<double>(0, 1));
			element = cv::getStructuringElement(cv::MORPH_CROSS, cv::Size(9, 9));
			cv::dilate(drawing, drawing, element);
			cv::erode(drawing, drawing, element);
			std::vector<std::vector<cv::Point> > hull(1);
			cv::convexHull(cv::Mat(contours[i]), hull[0], false);
			//cv::drawContours(drawing, contours, i, cv::Scalar(255, 255, 255), 1);
			//Scalar color = Scalar(255, 0, 0);
			//circle(drawing, hull[0][1], 3, color, 3);
			//color = Scalar(255, 255, 1);
			//circle(drawing, hull[0][hull[0].size() - 2], 3, color, 3);
		}
	}

	cv::Mat image = drawing.clone();

	cv::Mat input, output;
	input = drawing.clone();
	cv::cvtColor(input, output, CV_GRAY2BGR);

	int neighbors;
	clock_t tStart = clock();
	vector<Point> vec;
	for (int i = 1; i < (image.rows) - 1; i++)
	{
		for (int j = 1; j < (image.cols) - 1; j++)
		{

			if (image.at<uchar>(i, j) != 0)
			{
				neighbors = 0;

				for (int vy = i - 2; vy <= i + 2; vy++)
				{
					if (vy < 0)vy = 0;
					for (int vx = j - 2; vx <= j + 2; vx++)
					{
						if (vx < 0)vx = 0;
						if (vy == i && vx == j)
						{
							continue;
						}
						else
						{
							if (image.at<uchar>(vy, vx) != 0)
							{
								neighbors++;
							}
						}
					}
				}

				if (neighbors == 2)
				{
					vec.push_back(Point(j,i));
					//image.at<uchar>(i, j) = 80;
					//output.at<cv::Vec3b>(i, j) = cv::Vec3b(0, 0, 255);
				}
			}
		}
	}
	for each (Point p in vec)
	{
		Scalar color = Scalar(255, 0, 0);
		circle(output, p, 2, color, 2);
	}
	Scalar color = Scalar(0, 255, 0);
	circle(output, pCenter, 2, color, 2);
	//cv::imshow("mo", output);
	double a = ((double)(clock() - tStart) / CLOCKS_PER_SEC); /* 1s = 1000ms */
	cout << a << std::endl;
	cv::resize(output, output, cv::Size(300, 300), CV_INTER_NN);
	cv::resize(input, input, cv::Size(300, 300), CV_INTER_NN);
	cv::imshow("input", input);
	cv::imshow("output", output);
	/*cv::cvtColor(drawing, drawing, CV_BGR2GRAY);

	goodFeaturesToTrack(drawing,
	corners,
	maxCorners,
	qualityLevel,
	minDistance,
	Mat(),
	blockSize,
	useHarrisDetector,
	k);

	for (int i = 0; i < corners.size(); i++)
	{
	circle(drawing, corners[i], 3, Scalar(255, 0, 0))
	;
	}*/
	//imshow("Contours", drawing);

	//cv::imshow("Skeleton", skel);
	cv::waitKey(0);
	return 0;
}
