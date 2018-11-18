// main.cpp

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<conio.h>           // it may be necessary to change or remove this line if not using Windows
#include <thread> 
#include "Blob.h"
#include <stdio.h>
#include "wtypes.h"
#define SHOW_STEPS            // un-comment or comment this line to show steps or not
using namespace cv;
using namespace std;
// global variables ///////////////////////////////////////////////////////////////////////////////
const cv::Scalar SCALAR_BLACK = cv::Scalar(0.0, 0.0, 0.0);
const cv::Scalar SCALAR_WHITE = cv::Scalar(255.0, 255.0, 255.0);
const cv::Scalar SCALAR_YELLOW = cv::Scalar(0.0, 255.0, 255.0);
const cv::Scalar SCALAR_GREEN = cv::Scalar(0.0, 200.0, 0.0);
const cv::Scalar SCALAR_RED = cv::Scalar(0.0, 0.0, 255.0);

// function prototypes ////////////////////////////////////////////////////////////////////////////
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs);
void addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex);
void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs);
double distanceBetweenPoints(cv::Point point1, cv::Point point2);
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName);
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName);
bool checkIfBlobsCrossedTheLine(std::vector<Blob> &blobs, int &intHorizontalLinePosition, int &carCount, cv::Mat &imgFrame2Copy);
void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy);
void drawCarCountOnImage(int &carCount, cv::Mat &imgFrame2Copy);
std::vector<Blob> blobs;

cv::Point crossingLine[2];

int carCount = 0;
char chCheckForEscKey = 0;
bool blnFirstFrame = true;
int frameCount = 2;
int intHorizontalLinePosition = 0;
Mat imgFil;
Mat imgTracking1;
Mat imgTracking2;
Mat raw;
bool isFil = false;
VideoCapture	capVideo(1);
bool blLoad;
Mat imgThresh;
std::vector<std::vector<cv::Point> > contours;
cv::Mat imgFrame2Copy;
cv::Rect  recReusult;
cv::Rect  recCrop(265,10,200,470);
std::vector<std::vector<cv::Point> > list_contourTracking;
vector<Mat > list_imgTracking;
vector<Rect > list_recTracking;
vector<string > list_Result;
vector<vector<cv::Point> > contoursSampling;
Point pBE2, pBE1, pHut, pFist, center;
int szSampling = 90;
Mat imgResult;
Size szRaw;
cv::Point OffSet(cv::Point pInput, cv::Point pOffset)
{
	cv::Point point;
	point.x = pInput.x + pOffset.x;
	point.y = pInput.y + pOffset.y;
	return point;
}
void FindSampling()
{
	cv::String s = "sampling\\sampling.png";
	contoursSampling = vector<vector<cv::Point> >();
	Mat imgRaw = cv::imread(s, COLOR_BGR2GRAY);

	Mat imgGray = imgRaw;
	Mat imgThres = Mat();
	blur(imgGray, imgGray, cv::Size(10, 10), cv::Point(-1, -1));
	threshold(imgGray, imgThres, 180, 255, THRESH_BINARY);

	//bitwise_not(imgThres, imgThres);
	//imshow("loc3", imgThres);
	Mat outPut = Mat();
	cv::Canny(imgThres, outPut, 0, 255);
	//	imshow("loc2", imgThres);
	// Tạo biến ảnh so sánh biên dạng viền //vs các kích thước hình dáng khác nhau để đóng kín các vòng lặp chưa đóng kín
	Mat structuringElement = cv::getStructuringElement(CV_SHAPE_ELLIPSE, cv::Size(1, 1), cv::Point(0, 0));
	// đóng kín các biên dạng bằng biến ảnh so sánh ở trên
	cv::morphologyEx(outPut, outPut, 3, structuringElement);

	Mat hierarchy = Mat();
	findContours(outPut, contoursSampling, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));



}

void drawAxis(Mat& img, cv::Point p, cv::Point q, Scalar colour, const float scale = 0.2)
{
	double angle = atan2((double)p.y - q.y, (double)p.x - q.x); // angle in radians
	double hypotenuse = sqrt((double)(p.y - q.y) * (p.y - q.y) + (p.x - q.x) * (p.x - q.x));
	// Here we lengthen the arrow by a factor of scale
	q.x = (int)(p.x - scale * hypotenuse * cos(angle));
	q.y = (int)(p.y - scale * hypotenuse * sin(angle));
	line(img, p, q, colour, 1, LINE_AA);
	// create the arrow hooks
	p.x = (int)(q.x + 9 * cos(angle + CV_PI / 4));
	p.y = (int)(q.y + 9 * sin(angle + CV_PI / 4));
	line(img, p, q, colour, 1, LINE_AA);
	p.x = (int)(q.x + 9 * cos(angle - CV_PI / 4));
	p.y = (int)(q.y + 9 * sin(angle - CV_PI / 4));
	line(img, p, q, colour, 1, LINE_AA);
}
double AngleBetweenThreePoints(cv::Point pointA, cv::Point pointB, cv::Point pointC)
{
	float a = pointB.x - pointA.x;
	float b = pointB.y - pointA.y;
	float c = pointB.x - pointC.x;
	float d = pointB.y - pointC.y;
	float atanA = atan2(a, b);
	float atanB = atan2(c, d);
	return (atanB - atanA) * 180 / 3.14;
}
void analysisContour(const vector<cv::Point> &pts, Mat &img, cv::Rect boundingBox)
{	
	cv::Point cntr;
	center = cv::Point(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);
	int sz = static_cast<int>(pts.size());
	Mat data_pts = Mat(sz, 2, CV_64F);
	for (int i = 0; i < data_pts.rows; i++)
	{
		data_pts.at<double>(i, 0) = pts[i].x;
		data_pts.at<double>(i, 1) = pts[i].y;
	}
	PCA pca_analysis(data_pts, Mat(), PCA::DATA_AS_ROW);
	//Store the center of the object
	cntr = cv::Point(static_cast<int>(pca_analysis.mean.at<double>(0, 0)),
		static_cast<int>(pca_analysis.mean.at<double>(0, 1)));

	int indexQuadrant = 0;

	// Draw the principal components
	
	circle(img, cntr, 3, Scalar(255, 0, 255), 2);
	double distance2 = 2000, tAngle2;
	double distance3 = 2000;
	cv::Point curved_point2;
	int index = 0;
	bool blPoint = false, bl1 = false, bl2 = false;
	cv::Point pCenter;
	bool blAgain = false;
	int index1, index2, indexN = 0;
	distance2 = cv::pointPolygonTest(pts, cntr, true);
	int indexCenter = 0;
	double tempCT = 5000;


	for (int j = 0; j < pts.size(); j++)
	{
		double a = 0;

		a = sqrt(pow(pts[j].x - cntr.x, 2) + pow(pts[j].y - cntr.y, 2));

		if (a < tempCT)
		{
			tempCT = a;
			pCenter = pts[j];
			indexCenter = j;
		}
	}
	vector<vector<cv::Point> > c1(list_contourTracking.size());
	vector<vector<cv::Point> > c2(list_contourTracking.size());
	vector<vector<cv::Point> > c3(list_contourTracking.size());
	vector<vector<cv::Point> > c4(list_contourTracking.size());
	int minarea = 10;
	for (int j = 0; j < list_contourTracking[0].size(); j++)
	{
		if (boundingBox.width > boundingBox.height)
		{
			if (list_contourTracking[0][j].x >= pCenter.x - minarea && list_contourTracking[0][j].x <= pCenter.x + minarea && list_contourTracking[0][j].y <= pCenter.y)
				c1[0].push_back(list_contourTracking[0][j]);
			if (list_contourTracking[0][j].x >= pCenter.x - minarea && list_contourTracking[0][j].x <= pCenter.x + minarea && list_contourTracking[0][j].y >= pCenter.y)
				c2[0].push_back(list_contourTracking[0][j]);
		}
		else
		{
			if (list_contourTracking[0][j].y >= pCenter.y - minarea && list_contourTracking[0][j].y <= pCenter.y + minarea && list_contourTracking[0][j].x <= pCenter.x)
				c1[0].push_back(list_contourTracking[0][j]);
			if (list_contourTracking[0][j].y >= pCenter.y - minarea && list_contourTracking[0][j].y <= pCenter.y + minarea && list_contourTracking[0][j].x >= pCenter.x)
				c2[0].push_back(list_contourTracking[0][j]);
		}
	}
	Mat hierarchy;
	//cv::drawContours(outPut, c1, i, cv::Scalar(255, 0, 255), 1, 8, hierarchy, 1, cv::Point(xCrop, yCrop));
	//cv::drawContours(outPut, c2, i, cv::Scalar(0, 255, 255), 1, 8, hierarchy, 1, cv::Point(xCrop, yCrop));
	double area1 = contourArea(c1[0]);
	double area2 = contourArea(c2[0]);
	if (area1 > area2)
	{
		if (boundingBox.width < boundingBox.height)
		{
			indexQuadrant = 0;
		}
		else
		{
			indexQuadrant = 2;
		}

	}
	else
	{
		if (boundingBox.width < boundingBox.height)
		{
			indexQuadrant = 1;
		}
		else
		{
			indexQuadrant = 3;
		}
	}
	vector<cv::Point> listPointU;
	vector<cv::Point> listPointD;
	int num = 0;
	int indexMin = 1000;
	bool blBreak = false;
	index = 1;
	bool bl3 = false;
	bool bl4 = false;
	while (true)
	{
		cv::Point ct = cntr;
		if (indexQuadrant == 0)
			ct.x -= index;
		if (indexQuadrant == 1)
			ct.x += index;
		if (indexQuadrant == 2)
			ct.y -= index;
		if (indexQuadrant == 3)
			ct.y += index;
		distance2 = cv::pointPolygonTest(pts, ct, false);
		if (distance2 == 1 && bl3 == false){
			bl3 = true;
		}
		if (distance2 == -1 && bl3 == true){
			bl4 = true;
		}
		if (bl4 == true)
		{
			if (index < indexMin)indexMin = index;

			cv::Point pMind;
			pMind.x = (ct.x + cntr.x) / 2;
			pMind.y = (ct.y + cntr.y) / 2;
			pHut = pMind;
					
			cv::Point pDraw = OffSet(pMind, cv::Point(recCrop.x, recCrop.y));
			cv::circle(img, pDraw, 2, cv::Scalar(0, 255, 0), 6);
			break;
		}
		if (index > 500)
		{
			blBreak = true;
			break;
		}
		index++;
	}
	for (int j = indexCenter; true; j += 1)
	{
		int i1 = j;
		if (i1 > pts.size() - 1)
			i1 = abs(i1 - pts.size());
		index = 1;
		bool bl1 = false;
		bool bl2 = false;
		while (true)
		{
			cv::Point ct = pts[i1];
			if (indexQuadrant == 0)
				ct.x -= index;
			if (indexQuadrant == 1)
				ct.x += index;
			if (indexQuadrant == 2)
				ct.y -= index;
			if (indexQuadrant == 3)
				ct.y += index;
			distance2 = cv::pointPolygonTest(pts, ct, false);
			if (distance2 == 1 && bl1 == false){
				bl1 = true;
			}
			if (distance2 == -1 && bl1 == true){
				bl2 = true;
			}
			if (bl2 == true)
			{
				if (index < indexMin)indexMin = index;

				cv::Point pMind;
				pMind.x = (ct.x + pts[i1].x) / 2;
				pMind.y = (ct.y + pts[i1].y) / 2;
				pBE1 = pMind;
				listPointU.push_back(pMind);
				cv::Point pDraw = OffSet(pMind, cv::Point(recCrop.x, recCrop.y));
				cv::circle(img, pDraw, 1, cv::Scalar(0, 255, 0), 1);
				break;
			}
			if (index > 500)
			{
				blBreak = true;
				break;
			}
			index++;
		}


		if (blBreak == true)break;

	}
	num = 0; blBreak = false; indexMin = 1000;
	for (int j = indexCenter; true; j -= 1)
	{
		int i1 = j;
		if (i1 < 0)
			i1 = abs(i1 + pts.size());
		index = 1;
		bool bl1 = false;
		bool bl2 = false;

		while (true)
		{
			cv::Point ct = pts[i1];
			if (indexQuadrant == 0)
				ct.x -= index;
			if (indexQuadrant == 1)
				ct.x += index;
			if (indexQuadrant == 2)
				ct.y -= index;
			if (indexQuadrant == 3)
				ct.y += index;
			distance2 = cv::pointPolygonTest(pts, ct, false);
			if (distance2 == 1 && bl1 == false){
				bl1 = true;
			}
			if (distance2 == -1 && bl1 == true){

				bl2 = true;
			}
			if (bl2 == true)
			{
				if (index < indexMin)indexMin = index;

				cv::Point pMind;

				pMind.x = (ct.x + pts[i1].x) / 2;
				pMind.y = (ct.y + pts[i1].y) / 2;
				pBE2 = pMind;
				cv::Point pDraw = OffSet(pMind, cv::Point(recCrop.x, recCrop.y));
				cv::circle(img, pDraw, 1, cv::Scalar(0, 255, 0), 1);
				listPointD.push_back(pMind);

				break;
			}
			if (index > 500)
			{
				blBreak = true;
				break;
			}
			index++;
		}

		num++;
		if (blBreak == true)break;

	}
	//vector<Moments> mu(list_contourTracking.size());
	//vector<cv::Point2f> mc(list_contourTracking.size());
	//mu[0] = moments(list_contourTracking[0]);
	//mc[0] = cv::Point2f(static_cast<float>(mu[0].m10 / (mu[0].m00)), static_cast<float>(mu[0].m01 / (mu[0].m00)));
	//tim hai diem dau duoi
	double lenMax1 = 2000, lenMax2 = 2000;
	double tLenMax1 = 10000, tLenMax2 = 10000;
	cv::Point pBeginEnd1 = pBE1;
	cv::Point pBeginEnd2 = pBE2;
	index1 = 0; index2 = 0;
	tLenMax1 = 1000;
	tLenMax2 = 1000;
	lenMax1 = 2000;
	lenMax2 = 2000;
	for (int k = 0; k < list_contourTracking[0].size(); k++)
	{
		lenMax1 = sqrt(pow(list_contourTracking[0][k].x - pBeginEnd1.x, 2) + pow(list_contourTracking[0][k].y - pBeginEnd1.y, 2));
		lenMax2 = sqrt(pow(list_contourTracking[0][k].x - pBeginEnd2.x, 2) + pow(list_contourTracking[0][k].y - pBeginEnd2.y, 2));

		if (lenMax1 < tLenMax1)
		{
			tLenMax1 = lenMax1;
			index1 = k;
		}
		if (lenMax2 < tLenMax2)
		{
			tLenMax2 = lenMax2;
			index2 = k;
		}
	}
	///Tao 2 bien dang contour dau duoi de so sanh vs contours mau de tim dau duoi
	std::vector<cv::Point> listcontoursUP;
	std::vector<cv::Point> listcontoursDown;
	vector<vector<cv::Point> > contoursUP(list_contourTracking.size());
	vector<vector<cv::Point> > contoursDown(list_contourTracking.size());
	cv::Point pdau1;
	cv::Point pdau2;
	int Sz = szSampling;
	int indexTren = 0;


	lenMax1 = 0;
	int i1 = 0, i2 = 0;
	//quet bien dang tren
	for (int k = index1; true; k++)
	{

		i1 = k;
		i2 = k + 1;
		if (i1 > list_contourTracking[0].size() - 1)
			i1 = abs(i1 - list_contourTracking[0].size());
		if (i2 > list_contourTracking[0].size() - 1)
			i2 = abs(i2 - list_contourTracking[0].size());
		lenMax1 += sqrt(pow(list_contourTracking[0][i1].x - list_contourTracking[0][i2].x, 2) + pow(list_contourTracking[0][i1].y - list_contourTracking[0][i2].y, 2));
		indexTren = i1;
		pdau1 = list_contourTracking[0][i1];
		if (lenMax1 > Sz) break;
	}
	int indexDuoi;	lenMax1 = 0;
	for (int k = index1; true; k--)
	{

		i1 = k;
		i2 = k - 1;
		if (i1 <0)
			i1 = abs(list_contourTracking[0].size() + i1);
		if (i2 <0)
			i2 = abs(list_contourTracking[0].size() + i2);

		lenMax1 += sqrt(pow(list_contourTracking[0][i1].x - list_contourTracking[0][i2].x, 2) + pow(list_contourTracking[0][i1].y - list_contourTracking[0][i2].y, 2));
		indexDuoi = i1;
		pdau2 = list_contourTracking[0][i1];
		if (lenMax1 > Sz) break;
	}

	for (int k = indexDuoi; true; k++)
	{
		i1 = k;

		if (i1 > list_contourTracking[0].size() - 1)
			i1 = abs(i1 - list_contourTracking[0].size());
		contoursUP[0].push_back(list_contourTracking[0][i1]);
		if (i1 == indexTren) break;
	}
	//quet bien dang duoi
	indexTren = 0;
	cv::Point pduoi1;
	cv::Point pduoi2;	lenMax1 = 0;
	for (int k = index2; true; k++)
	{
		i1 = k;
		i2 = k + 1;
		if (i1 > list_contourTracking[0].size() - 1)
			i1 = abs(i1 - list_contourTracking[0].size());
		if (i2 > list_contourTracking[0].size() - 1)
			i2 = abs(i2 - list_contourTracking[0].size());
		lenMax1 += sqrt(pow(list_contourTracking[0][i1].x - list_contourTracking[0][i2].x, 2) + pow(list_contourTracking[0][i1].y - list_contourTracking[0][i2].y, 2));
		indexTren = i1;
		pduoi1 = list_contourTracking[0][i1];
		if (lenMax1 > Sz) break;

	}

	indexDuoi = 0;	lenMax1 = 0;
	for (int k = index2; true; k--)
	{

		i1 = k;
		i2 = k - 1;
		if (i1 <0)
			i1 = abs(list_contourTracking[0].size() + i1);
		if (i2 <0)
			i2 = abs(list_contourTracking[0].size() + i2);
		lenMax1 += sqrt(pow(list_contourTracking[0][i1].x - list_contourTracking[0][i2].x, 2) + pow(list_contourTracking[0][i1].y - list_contourTracking[0][i2].y, 2));
		indexDuoi = i1;
		pduoi2 = list_contourTracking[0][i1];
		if (lenMax1 > Sz) break;
	}

	for (int k = indexDuoi; true; k++)
	{
		i1 = k;

		if (i1 > list_contourTracking[0].size() - 1)
			i1 = abs(i1 - list_contourTracking[0].size());
		contoursDown[0].push_back(list_contourTracking[0][i1]);

		if (i1 == indexTren) break;
	}

	//So sanh 2 bien dang tim duoc vs bien dang mau
	 area1 = 0; double tArea1 = 0;
    area2 = 0; double tArea2 = 0;
	cv::Point p1, p2, p3, p4;
/*	for (int k = 0; k < contoursUP[0].size(); k++)
	{
		if (k == contoursUP[0].size() - 1)
			area1 = sqrt(pow(contoursUP[0][k].x - contoursUP[0][0].x, 2) + pow(contoursUP[0][k].y - contoursUP[0][0].y, 2));
		else
			area1 = sqrt(pow(contoursUP[0][k].x - contoursUP[0][k + 1].x, 2) + pow(contoursUP[0][k].y - contoursUP[0][k + 1].y, 2));

		if (area1>tArea1)
		{
			tArea1 = area1;
			if (k == contoursUP[0].size() - 1)
			{
				p1 = contoursUP[0][k];
				p2 = contoursUP[0][0];
			}
			else
			{
				p1 = contoursUP[0][k];
				p2 = contoursUP[0][k + 1];
			}

		}
	}
	for (int k = 0; k < contoursDown[0].size(); k++)
	{
		if (k == contoursDown[0].size() - 1)
			area2 = sqrt(pow(contoursDown[0][k].x - contoursDown[0][0].x, 2) + pow(contoursDown[0][k].y - contoursDown[0][0].y, 2));
		else
			area2 = sqrt(pow(contoursDown[0][k].x - contoursDown[0][k + 1].x, 2) + pow(contoursDown[0][k].y - contoursDown[0][k + 1].y, 2));

		if (area2>tArea2)
		{
			tArea2 = area2;
			if (k == contoursDown[0].size() - 1)
			{
				p3 = contoursDown[0][k];
				p3 = contoursDown[0][0];
			}
			else
			{
				p3 = contoursDown[0][k];
				p4 = contoursDown[0][k + 1];
			}
		}
	}*/
	p1 = contoursUP[0][0];
	p2 = contoursUP[0][contoursUP[0].size()-2];
	p3 = contoursDown[0][0];
	p4 = contoursDown[0][contoursDown[0].size() - 2];
	double a1 = AngleBetweenThreePoints(p1, pBeginEnd1, p2);
	if (abs(a1) > 180) a1 = 360 - abs(a1);
	double a2 = AngleBetweenThreePoints(p3, pBeginEnd2, p4);
	if (abs(a2) > 180) a2 = 360 - abs(a2);
	a1 = abs(a1);
	a2 = abs(a2);
	double match1 = 0; double match2 = 0;
	cv::Point pDuoi;
	if (a1 > 60)
	{
		pFist = pBeginEnd2; pDuoi = pBeginEnd1;
	}
	else 	if (a2 > 60)
	{
		pFist = pBeginEnd1; pDuoi = pBeginEnd2;
	}

	else
	{
		match1 = round(cv::matchShapes(contoursUP[0], contoursSampling[0], CV_CONTOURS_MATCH_I1, 0) * 100);
		match2 = round(cv::matchShapes(contoursDown[0], contoursSampling[0], CV_CONTOURS_MATCH_I1, 0) * 100);
		if (match1 < match2) {
			pFist = pBeginEnd1; pDuoi = pBeginEnd2;
		}
		else {
			pFist = pBeginEnd2; pDuoi = pBeginEnd1;
		}
	}





	double angle;
	if (pFist.y < pHut.y && pFist.x > pHut.x)
		angle = atan2(pHut.y - pFist.y, pFist.x - pHut.x) * 180 / 3.14;
	else if (pFist.y < pHut.y && pFist.x < pHut.x)
		angle = 180 - atan2(pHut.y - pFist.y, pHut.x - pFist.x) * 180 / 3.14;
	else if (pFist.y > pHut.y && pFist.x < pHut.x)
		angle = 90 + atan2(pFist.y - pHut.y, pHut.x - pFist.x) * 180 / 3.14;
	else
		angle = 180 + atan2(pFist.y - pHut.y, pHut.x - pFist.x) * 180 / 3.14;
	//string Angle = "Goc:" + angle;

	angle = round(angle);
	int iangle = (int)angle;
	
	//Result res;

	//res.list(pHut.x, pHut.y, angle, true);
	//listResult.push_back(res);
	//result += to_string(pHut.x) + "," + to_string(pHut.y) + ",0" + "," + to_string(angle) + "\n";
	cv::Point pDraw; pDraw = cv::Point(center.x, center.y);
	cv::Point pDraw2 = cv::Point(pDraw.x + 10, pDraw.y - 10);
	//cv::circle(outPut, pDraw2, 9, cv::Scalar(0, 255, 0), 20);
	//conthu++;
	//	cv::putText(outPut, "1", pDraw, cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(0, 255, 0), 4); // Anti-alias (Optional)
	//DRAW     // color, 2, 8, hierarchy, 0, Point()
	//cv::drawContours(outPut, contours, i, cv::Scalar(0, 255, 0), 2, 8, hierarchy, 1, cv::Point(xCrop, yCrop));//vẽ biên dạng tìm được

	//cv::drawContours(raw, hull,0, cv::Scalar(0, 0, 255), 2, 8, hierarchy, 1); //vẽ biên dạng bao

	//	cv::circle(outPut, mc[i], 2, cv::Scalar(255, 0, 0), 4);//vẽ  vị trí tâm biên dạng tìm được

	//midpoint = OffSet(midpoint, cv::Point(xCrop, yCrop));
	//cv::circle(outPut, midpoint, 2, cv::Scalar(0, 255, 0), 4);
	//curved_point = OffSet(curved_point, cv::Point(xCrop, yCrop));
	//cv::circle(outPut, curved_point, 2, cv::Scalar(0, 255, 0), 4);
	//pHut = OffSet(pHut, cv::Point(xCrop, yCrop));
	//cv::circle(outPut, pHut, 4, cv::Scalar(0, 255, 0), 6);
	if (match1 < match2)
	{
		cv::drawContours(img, contoursUP, 0, cv::Scalar(255, 0, 0), 1, 8, hierarchy, 1, cv::Point(recCrop.x, recCrop.y));
		cv::drawContours(img, contoursDown, 0, cv::Scalar(255, 0, 0), 1, 8, hierarchy, 1, cv::Point(recCrop.x, recCrop.y));
	}
	else
	{
		cv::drawContours(img, contoursUP, 0, cv::Scalar(255, 0, 0), 1, 8, hierarchy, 1, cv::Point(recCrop.x, recCrop.y));
		cv::drawContours(img, contoursDown, 0, cv::Scalar(255, 0, 0), 1, 8, hierarchy, 1, cv::Point(recCrop.x, recCrop.y));
	}
	
	
	int isforward=0;
	if (boundingBox.width < boundingBox.height)
	{
		if (pFist.y>center.y)
		{
			if (pCenter.x >= center.x)
			{
				isforward = 0;
			}
			else
			{
				isforward = 1;
			}
		}
		else
		{
			if (pCenter.x >= center.x)
			{
				isforward =1;
			}
			else
			{
				isforward = 0;
			}
		}
	}
	else
	{
		if (pFist.x>center.x)
		{
			if (pCenter.y >= center.y)
			{
				isforward = 1;
			}
			else
			{
				isforward = 0;
			}
		}
		else
		{
			if (pCenter.y >= center.y)
			{
				isforward = 0;
			}
			else
			{
				isforward = 1;
			}
		}
	}
	string s = to_string(pHut.x) + "," + to_string(pHut.y) + "," + to_string(isforward) + "," + to_string(iangle);
	list_Result.push_back(s);
    pHut = OffSet(pHut, cv::Point(recCrop.x, recCrop.y));
	cv::circle(img, pHut, 4, cv::Scalar(255, 255, 0), 6);
	//cv::line(outPut, pHut, pFist, cv::Scalar(0, 255, 0), 2);
	//cv::line(outPut, pDuoi, pFist, cv::Scalar(0, 255, 0), 1);
	pFist = OffSet(pFist, cv::Point(recCrop.x, recCrop.y));
	cv::circle(img, pFist, 2, cv::Scalar(255, 0, 0), 2);
	pDuoi = OffSet(pDuoi, cv::Point(recCrop.x, recCrop.y));
	cv::circle(img, pDuoi, 4, cv::Scalar(0, 0, 255), 2);
	drawAxis(img, pHut, pFist, Scalar(0, 255, 0), 1);
	//	drawAxis(outPut, pHut, pDuoi, Scalar(255, 255, 0),1);
}
#pragma region Filter
void Filter()
{


	capVideo.read(raw);	
	raw.copyTo(imgFil);
	cv::rectangle(raw, recCrop, SCALAR_RED, 1);
	imgFil = imgFil(recCrop);
	cvtColor(imgFil, imgFil, COLOR_BGR2GRAY);
	cv::blur(imgFil, imgFil, Size(5,5));
	cv::threshold(imgFil, imgFil, 80, 255.0, CV_THRESH_BINARY);
	cv::Canny(imgFil, imgFil, 0, 255);
	Mat structuringElement = cv::getStructuringElement(CV_SHAPE_ELLIPSE, Size(10,10), cv::Point(-1, -1));
	cv::morphologyEx(imgFil, imgFil, 3, structuringElement);

}
#pragma endregion
void Tracking()
{

	isFil = false;

	imgThresh = imgFil.clone();
	std::vector<Blob> currentFrameBlobs;

	cv::Mat imgFrame1Copy = imgTracking1.clone();
	imgFrame2Copy = imgFil.clone();

	cv::Mat imgDifference;




	//cv::GaussianBlur(imgFrame1Copy, imgFrame1Copy, cv::Size(5, 5), 0);
	//cv::GaussianBlur(imgFrame2Copy, imgFrame2Copy, cv::Size(5, 5), 0);

	//cv::imshow(" 1.GaussianBlurs", imgFrame2Copy);
	
//	cv::absdiff(imgFrame1Copy, imgFrame2Copy, imgThresh);
	

	//for (unsigned int i = 0; i < 2; i++) {
	//	cv::dilate(imgThresh, imgThresh, structuringElement3x3);
	//	cv::dilate(imgThresh, imgThresh, structuringElement3x3);
	//	cv::erode(imgThresh, imgThresh, structuringElement3x3);
	//}


	cv::Mat imgThreshCopy = imgThresh.clone();


	std::vector<cv::Point> contour;

	cv::findContours(imgThreshCopy, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);



	std::vector<std::vector<cv::Point> > convexHulls(contours.size());

	for (unsigned int i = 0; i < contours.size(); i++) {
		double area = contourArea(contours[i]);
		if (area<10000 && area >90000) continue;
		cv::convexHull(contours[i], convexHulls[i]);
	}

//	drawAndShowContours(imgThresh.size(), convexHulls, "imgConvexHulls");
	int index = 0;
	for (auto &convexHull : convexHulls) {
		contour = contours[index]; index++;
		Blob possibleBlob(convexHull, contour);
		
		if (
			possibleBlob.currentBoundingRect.y>20&&
			possibleBlob.currentBoundingRect.area() > 9000 &&
			possibleBlob.currentBoundingRect.area() <50000 &&
			possibleBlob.dblCurrentAspectRatio > 0.3 &&
			possibleBlob.dblCurrentAspectRatio < 4.0 &&
			possibleBlob.currentBoundingRect.width > 10 &&
			possibleBlob.currentBoundingRect.height > 10 &&
			possibleBlob.dblCurrentDiagonalSize > 80 &&
			(cv::contourArea(possibleBlob.currentContour) / (double)possibleBlob.currentBoundingRect.area()) > 0.50) {
			currentFrameBlobs.push_back(possibleBlob);
		}
	}

	//drawAndShowContours(imgThresh.size(), currentFrameBlobs, "imgCurrentFrameBlobs");

	if (blnFirstFrame == true) {
		for (auto &currentFrameBlob : currentFrameBlobs) {
			blobs.push_back(currentFrameBlob);
		}
	}
	else {
		matchCurrentFrameBlobsToExistingBlobs(blobs, currentFrameBlobs);
	}

	//drawAndShowContours(imgThresh.size(), blobs, "imgBlobs");

	imgFrame2Copy = raw.clone();          // get another copy of frame 2 since we changed the previous frame 2 copy in the processing above

	  drawBlobInfoOnImage(blobs, imgFrame2Copy);

	bool blnAtLeastOneBlobCrossedTheLine = checkIfBlobsCrossedTheLine(blobs, intHorizontalLinePosition, carCount, imgFrame2Copy);

	if (blnAtLeastOneBlobCrossedTheLine == true) {
		cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_GREEN, 2);
	}
	else {
		cv::line(imgFrame2Copy, crossingLine[0], crossingLine[1], SCALAR_RED, 2);
	}





	//cv::waitKey(0);                 // uncomment this line to go frame by frame for debugging

	// now we prepare for the next iteration

	currentFrameBlobs.clear();

	//imgTracking1 = imgTracking2.clone();           // move frame 1 up to where frame 2 is
	blnFirstFrame = false;


}
void FindResult()
{
	if (list_contourTracking.size() != 0)
	{
		if (list_contourTracking.size() == list_imgTracking.size() && list_contourTracking.size() == list_recTracking.size())
		{
			analysisContour(list_contourTracking[0], list_imgTracking[0], list_recTracking[0]);
			cv::drawContours(list_imgTracking[0], list_contourTracking, 0, SCALAR_GREEN, 1);
			list_imgTracking[0].copyTo(imgResult);
			list_contourTracking.erase(list_contourTracking.begin());

			list_imgTracking.erase(list_imgTracking.begin());
			list_recTracking.erase(list_recTracking.begin());
			cv::Rect rec = recReusult;
			rec.x = recReusult.x + recCrop.x;
			rec.y = recReusult.y + recCrop.y;
			imgResult = imgResult(rec);
		}
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////
int main(void) {
	FindSampling();
	
	cv::Mat imgFrame1;

	capVideo.read(imgFrame1);
	szRaw = imgFrame1.size();

	capVideo.set(CV_CAP_PROP_SETTINGS, 1);
	intHorizontalLinePosition = (int)std::round((double)imgFrame1.rows * 0.4);

	crossingLine[0].x = 0;
	crossingLine[0].y = intHorizontalLinePosition;

	crossingLine[1].x = imgFrame1.cols - 1;
	crossingLine[1].y = intHorizontalLinePosition;




	while (chCheckForEscKey != 27) {
		thread proFilter(Filter);
		if (proFilter.joinable())
		{
			proFilter.join();
			isFil = true;
		}
		thread proTracking(Tracking);
		if (proTracking.joinable())
		{
			proTracking.join();
		}
		thread proFind(FindResult);
		if (proFind.joinable())
		{
			proFind.join();
		}
		cv::imshow(" 1.RAW", raw);
		cv::imshow(" 2.filter", imgFil);
		cv::imshow("imgThresh", imgThresh);
		drawAndShowContours(imgThresh.size(), contours, "imgContours");
		drawCarCountOnImage(carCount, imgFrame2Copy);
		cv::imshow("imgFrame2Copy", imgFrame2Copy);
		//capVideo.read(imgFil);
		//cv::imshow(" 1.filter", imgFil);
		cv::waitKey(1);


	}
	return(0);
	/*capVideo >> imgFrame2;

	capVideo.read(imgFrame2);

	frameCount++;
	chCheckForEscKey = cv::waitKey(1);
	}

	if (chCheckForEscKey != 27) {               // if the user did not press esc (i.e. we reached the end of the video)
	cv::waitKey(0);                         // hold the windows open to allow the "end of video" message to show
	}
	// note that if the user did press esc, we don't need to hold the windows open, we can simply let the program end which will close the windows
	*/

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void matchCurrentFrameBlobsToExistingBlobs(std::vector<Blob> &existingBlobs, std::vector<Blob> &currentFrameBlobs) {

	for (auto &existingBlob : existingBlobs) {

		existingBlob.blnCurrentMatchFoundOrNewBlob = false;

		existingBlob.predictNextPosition();
	}

	for (auto &currentFrameBlob : currentFrameBlobs) {

		int intIndexOfLeastDistance = 0;
		double dblLeastDistance = 100000.0;

		for (unsigned int i = 0; i < existingBlobs.size(); i++) {

			if (existingBlobs[i].blnStillBeingTracked == true) {

				double dblDistance = distanceBetweenPoints(currentFrameBlob.centerPositions.back(), existingBlobs[i].predictedNextPosition);

				if (dblDistance < dblLeastDistance) {
					dblLeastDistance = dblDistance;
					intIndexOfLeastDistance = i;
				}
			}
		}

		if (dblLeastDistance < currentFrameBlob.dblCurrentDiagonalSize * 0.5) {
			addBlobToExistingBlobs(currentFrameBlob, existingBlobs, intIndexOfLeastDistance);
		}
		else {
			addNewBlob(currentFrameBlob, existingBlobs);
		}

	}

	for (auto &existingBlob : existingBlobs) {

		if (existingBlob.blnCurrentMatchFoundOrNewBlob == false) {
			existingBlob.intNumOfConsecutiveFramesWithoutAMatch++;
		}

		if (existingBlob.intNumOfConsecutiveFramesWithoutAMatch >= 5) {
			existingBlob.blnStillBeingTracked = false;
		}

	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////
void addBlobToExistingBlobs(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs, int &intIndex) {
	existingBlobs[intIndex].currentContour2 = currentFrameBlob.currentContour2;
	existingBlobs[intIndex].currentContour = currentFrameBlob.currentContour;
	existingBlobs[intIndex].currentBoundingRect = currentFrameBlob.currentBoundingRect;

	existingBlobs[intIndex].centerPositions.push_back(currentFrameBlob.centerPositions.back());

	existingBlobs[intIndex].dblCurrentDiagonalSize = currentFrameBlob.dblCurrentDiagonalSize;
	existingBlobs[intIndex].dblCurrentAspectRatio = currentFrameBlob.dblCurrentAspectRatio;

	existingBlobs[intIndex].blnStillBeingTracked = true;
	existingBlobs[intIndex].blnCurrentMatchFoundOrNewBlob = true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void addNewBlob(Blob &currentFrameBlob, std::vector<Blob> &existingBlobs) {

	currentFrameBlob.blnCurrentMatchFoundOrNewBlob = true;

	existingBlobs.push_back(currentFrameBlob);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
double distanceBetweenPoints(cv::Point point1, cv::Point point2) {

	int intX = abs(point1.x - point2.x);
	int intY = abs(point1.y - point2.y);

	return(sqrt(pow(intX, 2) + pow(intY, 2)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawAndShowContours(cv::Size imageSize, std::vector<std::vector<cv::Point> > contours, std::string strImageName) {
	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

	cv::drawContours(image, contours, -1, SCALAR_WHITE, 1);

	cv::imshow(strImageName, image);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawAndShowContours(cv::Size imageSize, std::vector<Blob> blobs, std::string strImageName) {

	cv::Mat image(imageSize, CV_8UC3, SCALAR_BLACK);

	std::vector<std::vector<cv::Point> > contours;

	for (auto &blob : blobs) {
		if (blob.blnStillBeingTracked == true) {
			contours.push_back(blob.currentContour);
		}
	}

	cv::drawContours(image, contours, -1, SCALAR_WHITE, -1);

	cv::imshow(strImageName, image);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
bool checkIfBlobsCrossedTheLine(std::vector<Blob> &blobs, int &intHorizontalLinePosition, int &carCount, cv::Mat &imgFrame2Copy) {
	bool blnAtLeastOneBlobCrossedTheLine = false;

	for (auto blob : blobs) {

		if (blob.blnStillBeingTracked == true && blob.centerPositions.size() >= 2) {
			int prevFrameIndex = (int)blob.centerPositions.size() - 2;
			int currFrameIndex = (int)blob.centerPositions.size() - 1;

			if (blob.centerPositions[prevFrameIndex].y > intHorizontalLinePosition && blob.centerPositions[currFrameIndex].y <= intHorizontalLinePosition) {
				carCount++;
				blnAtLeastOneBlobCrossedTheLine = true;
				std::vector<std::vector<cv::Point>> contours2;
				contours2.push_back(blob.currentContour2);
				recReusult = blob.currentBoundingRect;
				list_contourTracking.push_back(blob.currentContour2);
				Mat rawcoppy;
				raw.copyTo(rawcoppy);
				list_imgTracking.push_back(rawcoppy);
				list_recTracking.push_back(recReusult);
				 
				
	
				
				
				//cv::rectangle(imgFrame2Copy, blob.currentBoundingRect, SCALAR_RED, 2);

				int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
				double dblFontScale = blob.dblCurrentDiagonalSize / 60.0;
				int intFontThickness = (int)std::round(dblFontScale * 1.0);
				//cv::imshow("OUT", blob.currentContour);

				//cv::putText(imgFrame2Copy, std::to_string(carCount), blob.centerPositions.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);

			}
		}

	}

	return blnAtLeastOneBlobCrossedTheLine;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawBlobInfoOnImage(std::vector<Blob> &blobs, cv::Mat &imgFrame2Copy) {

	for (unsigned int i = 0; i < blobs.size(); i++) {

		if (blobs[i].blnStillBeingTracked == true) {
			//, cv::Point(recCrop.x, recCrop.y)
			cv::Rect rec = blobs[i].currentBoundingRect;
			rec.x = rec.x + recCrop.x;
			rec.y = rec.y + recCrop.y;

			cv::rectangle(imgFrame2Copy, rec, SCALAR_RED, 2);

			//int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
			//double dblFontScale = blobs[i].dblCurrentDiagonalSize / 60.0;
			//int intFontThickness = (int)std::round(dblFontScale * 1.0);

			//cv::putText(imgFrame2Copy, std::to_string(i), blobs[i].centerPositions.back(), intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);
			//cv::imshow("OUT", imgFrame2Copy);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawCarCountOnImage(int &carCount, cv::Mat &imgFrame2Copy) {

	int intFontFace = CV_FONT_HERSHEY_SIMPLEX;
	double dblFontScale = (imgFrame2Copy.rows * imgFrame2Copy.cols) / 300000.0;
	int intFontThickness = (int)std::round(dblFontScale * 1.5);

	cv::Size textSize = cv::getTextSize(std::to_string(carCount), intFontFace, dblFontScale, intFontThickness, 0);

	cv::Point ptTextBottomLeftPosition;

	ptTextBottomLeftPosition.x = imgFrame2Copy.cols - 1 - (int)((double)textSize.width * 1.25);
	ptTextBottomLeftPosition.y = (int)((double)textSize.height * 1.25);
	if (recReusult.width != 0)
	{
		imgResult.copyTo(imgFrame2Copy(cv::Rect(10,10, recReusult.width, recReusult.height)));
	}
	int index = 50;
	int index2 = 0;
	for each (string s  in list_Result)
	{
		index2++;
		s ="("+to_string(index2) + ") "+s ;
		cv::putText(imgFrame2Copy, s, cv::Point(szRaw.width - 120, index), intFontFace, 0.3, SCALAR_GREEN, 1);
		index += 20;

	}
	cv::putText(imgFrame2Copy, std::to_string(carCount), ptTextBottomLeftPosition, intFontFace, dblFontScale, SCALAR_GREEN, intFontThickness);

}

