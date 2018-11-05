#include "wtypes.h"
#include <opencv2/core/core.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#define CVUI_IMPLEMENTATION
#include "lib/cvui.h"
#include "lib/EnhancedWindow.h"
#define wFILTER "FILTER"
#define wMain "VISION ONES"
#define WINDOW3_NAME "Windows 3"
#define WINDOW4_NAME "Windows 4"
#include <thread>
using namespace cv;
using namespace std;
bool selectObject = false;
Rect selection;
Point origin;
int trackObject = 0;
int valCCD1 = 0;
int valCCD2 = 0;
int valCCD3 = 0;
int valCCD4 = 0;
int valCCD5 = 0;
int valCCD6 = 0;
int tvalCCD1 = 0;
int tvalCCD2 = 0;
int tvalCCD3 = 0;
int tvalCCD4 = 0;
int tvalCCD5 = 0;
int tvalCCD6 = 0;
bool ckRGB = false;
bool tckRGB = false;
bool ck11 = false;
bool ck12 = false;
bool ck21 = false;
bool ck22 = false;
bool c31 = false;
bool ck32 = false;
bool ck41 = false;
bool ck42 = false;
bool ck6 = false;
bool blFilter = false;
bool blRun = false;
double szRectX = 0;
double szRectY = 0;
double szKenelX = 0;
double szKenelY = 0;
double szBlurX = 0;
double szBlurY = 0;
double typeBinary = 1;
double minBinary = 0;
cv::Point anchors[3]; // one anchor for each mouse button
cv::Rect rois[3]; // one ROI for each mouse button
unsigned int colors[] = { 0xff0000, 0x00ff00, 0x0000ff };
int xBlur = 100;
int yBlur = 50;
int xBinary = 100;
int yBinary = 100;
int xCanny = 100;
int yCanny = 120;
EnhancedWindow grBlur(xBlur, yBlur + 20, 200, 300, "Clear Blob", true, true, true);
EnhancedWindow grbinary(xBlur, yBlur + 40, 200, 280, "Binary", true, false, true);
EnhancedWindow grcanny(xBlur, yBlur + 60, 200, 260, "Canny", true, false, true);
int xCropArea = 100;
int yCropArea = 80;
int xCropTrigger = 100;
int yCropTrigger = 100;
int xSampling = 100;
int ySampling = 120;
EnhancedWindow grCropTrigger(xCropArea, yCropArea + 20, 250, 280, "Crop", true, false, true);
EnhancedWindow grSampling(xCropArea, yCropArea + 40, 250, 260, "Sampling", true, false, true);
EnhancedWindow grContour(xCropArea, yCropArea + 60, 250, 240, "Contour", true, false, true);

bool blSetting = true;
bool blTool = false;
bool blCom = false;
bool blResut = false;
bool blRecord = false;
String textIntput = "";
Mat frCap;

enum nbtnRecord
{
	Rec,
	Recorded
};
#pragma region GUI
void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}
Mat frame;
int height = 0;
int with = 0;
void fFilter(Mat frame)
{
	if (blFilter == true)
	{

		grcanny.begin(frame);
		if (!grcanny.isMinimized()) {
			cvui::beginRow(-1, -1, 50);
			cvui::checkbox("view", &ck21, 0x1cbaf6);
			cvui::checkbox("using", &ck22);//, 0xff0000);
			cvui::endRow();
			cvui::space(10);
			cvui::text("Size Close", 0.4, 0x8ccbfb);
			cvui::space(10);
			cvui::beginRow(-1, -1, 90);
			cvui::text("with", 0.3, 0xb2a6a6);
			cvui::text("height", 0.3, 0xb2a6a6);
			cvui::endRow();
			cvui::space(5);
			cvui::beginRow(-1, -1, 5);
			cvui::counter(&szRectX, 1, "%.0f");
			cvui::counter(&szRectY, 1, "%.0f");
			cvui::endRow();
			//
		}
		grcanny.end();
		grbinary.begin(frame);
		if (!grbinary.isMinimized()) {
			cvui::beginRow(-1, -1, 50);
			//cvui::text("");
			//cvui::space(1);
			cvui::checkbox("view", &ck21, 0x1cbaf6);
			cvui::checkbox("using", &ck22);//, 0xff0000);
			cvui::endRow();
			cvui::space(10);
			cvui::beginColumn(-1, -1, 6);
			cvui::text("Type", 0.5, 0x148ae3);
			cvui::trackbar(180, &typeBinary, 1., 4., 2, "%.0Lf", cvui::TRACKBAR_DISCRETE | cvui::TRACKBAR_HIDE_SEGMENT_LABELS, 1.0);
			cvui::text("Value Min", 0.5, 0x148ae3);
			cvui::trackbar(180, &minBinary, 0., 255., 1, "%.0Lf", cvui::TRACKBAR_DISCRETE | cvui::TRACKBAR_HIDE_SEGMENT_LABELS, 1.0);
			cvui::endColumn();
		}
		grbinary.end();
		grBlur.begin(frame);
		if (!grBlur.isMinimized()) {
			cvui::beginRow(-1, -1, 50);
			//cvui::space(1);
			cvui::checkbox("view", &ck11, 0x1cbaf6);
			cvui::checkbox("using", &ck12);//, 0xff0000);
			cvui::endRow();

			cvui::space(10);
			cvui::text("Size Rec", 0.4, 0x8ccbfb);
			cvui::space(10);
			cvui::beginRow(-1, -1, 90);
			cvui::text("with", 0.3, 0xb2a6a6);
			cvui::text("height", 0.3, 0xb2a6a6);
			cvui::endRow();
			cvui::space(5);
			cvui::beginRow(-1, -1, 5);
			cvui::counter(&szRectX, 1, "%.0f");
			cvui::counter(&szRectY, 1, "%.0f");
			cvui::endRow();
			//
			cvui::space(10);
			cvui::text("Size Kenel", 0.4, 0x8ccbfb);
			cvui::space(10);
			cvui::beginRow(-1, -1, 90);
			cvui::text("with", 0.3, 0xb2a6a6);
			cvui::text("height", 0.3, 0xb2a6a6);
			cvui::endRow();
			cvui::space(5);
			cvui::beginRow(-1, -1, 5);
			cvui::counter(&szKenelX, 1, "%.0f");
			cvui::counter(&szKenelY, 1, "%.0f");
			cvui::endRow();
			//
			cvui::space(10);
			cvui::text("Size Blur", 0.4, 0x8ccbfb);
			cvui::space(10);
			cvui::beginRow(-1, -1, 90);
			cvui::text("with", 0.3, 0xb2a6a6);
			cvui::text("height", 0.3, 0xb2a6a6);
			cvui::endRow();
			cvui::space(5);
			cvui::beginRow(-1, -1, 5);
			cvui::counter(&szBlurX, 1, "%.0f");
			cvui::counter(&szBlurY, 1, "%.0f");
			cvui::endRow();
		}
		grBlur.end();

	}
}
void fTool(Mat frame)
{
	if (blTool == true)
	{

		grContour.begin(frame);
		if (!grContour.isMinimized()) {

			cvui::space(10);
			cvui::text("Limit size contour", 0.4, 0x8ccbfb);
			cvui::space(20);
			cvui::beginRow(-1, -1, 1);
			cvui::space(10);
			cvui::beginColumn(-1, -1, 1);
			cvui::text("Min size", 0.4, 0xb2a6a6);
			cvui::space(5);
			cvui::counter(&szRectX, 1, "%.0f");
			cvui::space(5);
			cvui::text("Max size", 0.4, 0xb2a6a6);
			cvui::space(5);
			cvui::counter(&szRectY, 1, "%.0f");
			cvui::endColumn();
			cvui::endRow();

			//
		}
		grContour.end();
		grSampling.begin(frame);
		if (!grSampling.isMinimized()) {
			cvui::beginRow(-1, -1, 0);
			//cvui::image(frFilter);
			cvui::space(10);
			cvui::beginColumn(-1, -1, 5);
			cvui::text("Limit sampling", 0.4, 0x8ccbfb);
			cvui::space(5);
			cvui::counter(&szRectX, 1, "%.0f");
			cvui::space(5);
			cvui::button(90, 40, "Crop");
			cvui::endColumn();
			cvui::endRow();
			cvui::space(5);
			cvui::button(220, 30, "Apply");
		}
		grSampling.end();
		grCropTrigger.begin(frame);
		if (!grCropTrigger.isMinimized()) {

			cvui::text("Crop Area", 0.4, 0x8ccbfb);
			cvui::space(10);
			cvui::beginRow(-1, -1, 10);
			cvui::space(5);
			cvui::beginColumn(-1, -1, 5);
			cvui::beginRow(-1, -1, 5);
			cvui::text("Point", 0.35, 0xfef0aa);
			cvui::space(2);
			cvui::text("x", 0.5, 0xec037a);
			cvui::space(20);
			cvui::text("y", 0.5, 0x51aec7);
			cvui::endRow();

			cvui::beginRow(-1, -1, 5);
			cvui::text("Size", 0.35, 0xfef0aa);
			cvui::space(2);
			cvui::text("with", 0.5, 0xec037a);
			cvui::space(20);
			cvui::text("height", 0.5, 0x51aec7);
			cvui::endRow();
			//cvui::space(5);
			cvui::button(150, 20, "CROP AREA");
			cvui::endColumn();
			cvui::endRow();
			cvui::space(10);
			cvui::text("Crop Trigger", 0.4, 0x8ccbfb);
			cvui::space(10);
			cvui::beginRow(-1, -1, 10);
			cvui::space(5);
			cvui::beginColumn(-1, -1, 5);
			cvui::beginRow(-1, -1, 5);
			cvui::text("Point", 0.35, 0xfef0aa);
			cvui::space(2);
			cvui::text("x", 0.5, 0xec037a);
			cvui::space(20);
			cvui::text("y", 0.5, 0x51aec7);
			cvui::endRow();

			cvui::beginRow(-1, -1, 5);
			cvui::text("Size", 0.35, 0xfef0aa);
			cvui::space(2);
			cvui::text("with", 0.5, 0xec037a);
			cvui::space(20);
			cvui::text("height", 0.5, 0x51aec7);
			cvui::endRow();
			//cvui::space(5);
			cvui::button(150, 20, "Crop Trigger");
			cvui::endColumn();
			cvui::endRow();
		}
		grCropTrigger.end();

	}
}
void fSetLocation()
{

	xBlur = grBlur.X();
	yBlur = grBlur.Y();
	grbinary.setX(xBlur);
	grbinary.setY(yBlur + 20);
	grcanny.setX(xBlur);
	grcanny.setY(yBlur + 40);
}
void fComunication(Mat frame, int x, int y, int w, int h)
{
	cvui::window(frame, x, y, w, h, "Comunication");
	cvui::text(frame, x + 10, y + 30, "CCD", 0.36, 0xb2a6a6);
	cvui::counter(frame, x + 60, y + 30, &szRectX, 1, "%.0f");
	cvui::button(frame, x + 180, y + 30, 110, 20, "CHANGE");

	cvui::text(frame, x + 10, y + 70, "Robot Comunication", 0.36, 0xb2a6a6);
	cvui::text(frame, x + 20, y + 90, "IP Address", 0.36, 0xb2a6a6);
	cvui::text(frame, x + 90, y + 90, "192.168.1.99:1025", 0.4, 0x0b9bf2);
	cvui::button(frame, x + 230, y + 90, 60, 20, "SCAN");

	cvui::text(frame, x + 10, y + 130, "Port", 0.36, 0xb2a6a6);
	cvui::counter(frame, x + 60, y + 130, &szRectX, 1, "%.0f");
	cvui::button(frame, x + 180, y + 130, 110, 20, "CHANGE");
}
void fResult(Mat frame)
{
	cvui::window(frame, 3 * with / 4, 0, 1 * with / 4, height, "Result");

}
#pragma endregion 
double cycleTrigger = 0;
#pragma region VISION
#pragma region Variable
vector<vector<cv::Point> > contours;
cv::Rect boundingBox;
cv::Point center;
cv::Point pBE1;
cv::Point pBE2;
cv::Point pHut;
vector<vector<cv::Point> > contoursSampling;
Mat frTrigger;
Mat frFilter;
Mat frFilter1;
Mat frContour1;
Mat input;
int  szRectW = 1;
int  szRectH = 50;
int szKernelW = 30;
int szKernelH = 36;
int szBlurW = 10;
int szBlurH =5;
int szThreshW = 180;
int szThreshH = 255;
float areaTriger = 0;
bool ck1 = true;
bool ck2 = true;
bool ck3 = true;
Mat matTrigger;
#pragma endregion
#pragma region Function

class Result
{
public: int x = 0, y = 0, del = 0;
public: bool rev;
public:	void list(int _x, int _y, int _del, bool _rev) {

			x = _x;
			y = _y;
			del = _del;
			rev = _rev;
}
};
vector<Result> listResult;
cv::Size convertSZ(int W, int H)
{
	cv::Size sz = cv::Size(W, H);
	return sz;
}
cv::Point OffSet(cv::Point pInput, cv::Point pOffset)
{
	cv::Point point;
	point.x = pInput.x + pOffset.x;
	point.y = pInput.y + pOffset.y;
	return point;
}
void drawAxis(Mat&, cv::Point, cv::Point, Scalar, const float);
double getOrientation(const vector<cv::Point> &, Mat&);
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
void getOrientation(const vector<cv::Point> &pts, Mat &img, int xCrop, int yCrop, int i)
{
	cv::Point cntr;
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
	cv::Point p4 = OffSet(cntr, cv::Point(xCrop, yCrop));
	circle(img, p4, 3, Scalar(255, 0, 255), 2);
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
	vector<vector<cv::Point> > c1(contours.size());
	vector<vector<cv::Point> > c2(contours.size());
	vector<vector<cv::Point> > c3(contours.size());
	vector<vector<cv::Point> > c4(contours.size());
	int minarea = 10;
	for (int j = 0; j < contours[i].size(); j++)
	{
		if (boundingBox.width > boundingBox.height)
		{
			if (contours[i][j].x >= pCenter.x - minarea && contours[i][j].x <= pCenter.x + minarea && contours[i][j].y <= pCenter.y)
				c1[i].push_back(contours[i][j]);
			if (contours[i][j].x >= pCenter.x - minarea && contours[i][j].x <= pCenter.x + minarea && contours[i][j].y >= pCenter.y)
				c2[i].push_back(contours[i][j]);
		}
		else
		{
			if (contours[i][j].y >= pCenter.y - minarea && contours[i][j].y <= pCenter.y + minarea && contours[i][j].x <= pCenter.x)
				c1[i].push_back(contours[i][j]);
			if (contours[i][j].y >= pCenter.y - minarea && contours[i][j].y <= pCenter.y + minarea && contours[i][j].x >= pCenter.x)
				c2[i].push_back(contours[i][j]);
		}
	}
	Mat hierarchy;
	//cv::drawContours(outPut, c1, i, cv::Scalar(255, 0, 255), 1, 8, hierarchy, 1, cv::Point(xCrop, yCrop));
	//cv::drawContours(outPut, c2, i, cv::Scalar(0, 255, 255), 1, 8, hierarchy, 1, cv::Point(xCrop, yCrop));
	double area1 = contourArea(c1[i]);
	double area2 = contourArea(c2[i]);
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
			cv::Point pDraw1;
			pDraw1 = OffSet(pMind, cv::Point(xCrop, yCrop));
			cv::circle(img, pDraw1, 2, cv::Scalar(0, 255, 0), 6);
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
				cv::Point p2 = OffSet(pMind, cv::Point(xCrop, yCrop));
				cv::circle(img, p2, 1, cv::Scalar(0, 255, 0), 1);
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
				cv::Point p1 = OffSet(pMind, cv::Point(xCrop, yCrop));
				cv::circle(img, p1, 1, cv::Scalar(0, 255, 0), 1);
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

	/*cv::Point p1 = cntr - 0.02 * cv::Point(static_cast<int>(eigen_vecs[0].x * eigen_val[0]), static_cast<int>(eigen_vecs[0].y * eigen_val[0]));
	cv::Point p2 = cntr + 0.02 * cv::Point(static_cast<int>(eigen_vecs[1].x * eigen_val[1]), static_cast<int>(eigen_vecs[1].y * eigen_val[1]));
	*/


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
class Parallel_process : public cv::ParallelLoopBody
{

private:
	cv::Mat img;
	cv::Mat& retVal;
	int size;
	int diff;

public:
	Parallel_process(cv::Mat inputImgage, cv::Mat& outImage,
		int sizeVal, int diffVal)
		: img(inputImgage), retVal(outImage),
		size(sizeVal), diff(diffVal){}

	virtual void operator()(const cv::Range& range) const
	{
		for (int i = range.start; i < range.end; i++)
		{

			cv::Mat in(img, cv::Rect(0, (img.rows / diff)*i,
				img.cols, img.rows / diff));
			areaTriger = countNonZero(in);
			//out = in;
			//areaTriger = countNonZero(in);


		}
	}
};

#pragma endregion
#pragma region A FILTER
Mat ClearBlob(Mat inPut, cv::Size szRect, cv::Size szKernel, cv::Size szBlur, bool ck)
{
	Mat vertical = Mat();
	Mat bw = inPut;
	vertical = bw.clone();
	// Specify size on horizontal axis
	int verticalsize = vertical.rows / szRect.height;
	// Create structure element for extracting vertical lines through morphology operations
	Mat verticalStructure = getStructuringElement(MORPH_RECT, cv::Size(szRect.width, verticalsize));
	// Apply morphology operations
	erode(vertical, vertical, verticalStructure, cv::Point(-1, -1));
	dilate(vertical, vertical, verticalStructure, cv::Point(-1, -1));
	// Show extracted vertical lines
	//imshow("vertical", vertical);
	// Inverse vertical image
	bitwise_not(vertical, vertical);
	//imshow("vertical_bit", vertical);
	// Extract edges and smooth image according to the logic
	// 1. extract edges
	// 2. dilate(edges)
	// 3. src.copyTo(smooth)
	// 4. blur smooth img
	// 5. smooth.copyTo(src, edges)
	// Step 1
	Mat edges;
	adaptiveThreshold(vertical, edges, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, -2);
	//imshow("edges", edges);
	// Step 2
	Mat kernel = Mat::ones(szKernel.width, szKernel.height, CV_8UC1);
	dilate(edges, edges, kernel);
	//imshow("dilate", edges);
	// Step 3
	Mat smooth;
	vertical.copyTo(smooth);
	// Step 4
	blur(smooth, smooth, szBlur);
	// Step 5
	smooth.copyTo(vertical, edges);
	// Show final result

	if (ck == true)	imshow("ClearBlob", vertical);
	//threshold(vertical, vertical, 150, 255, THRESH_BINARY);
	//imshow("BINARY", vertical);
	return vertical;
}
Mat Thresh_binary(Mat inPut, cv::Size szThresh, bool ck)
{
	threshold(inPut, inPut, szThresh.width, szThresh.height, THRESH_BINARY);
	if (ck == true) imshow("BINARY", inPut);
	return inPut;
}
Mat Canny(Mat inPut, cv::Size szCanny, bool ck)
{
	Mat outPut = Mat();
	cv::Canny(inPut, inPut, 0, 255);
	// Tạo biến ảnh so sánh biên dạng viền //vs các kích thước hình dáng khác nhau để đóng kín các vòng lặp chưa đóng kín
	Mat structuringElement = cv::getStructuringElement(CV_SHAPE_ELLIPSE, szCanny, cv::Point(-1, -1));
	// đóng kín các biên dạng bằng biến ảnh so sánh ở trên
	cv::morphologyEx(inPut, outPut, 3, structuringElement);
	if (ck == true) imshow("Canny", outPut);
	return outPut;
}
#pragma endregion
#pragma region FILTERs
Mat Filter(Mat input, int  szRectW, int  szRectH, int szKernelW, int szKernelH, int szBlurW, int szBlurH, int szThreshW, int szThreshH, int szClose_x, int szClose_y, bool ck1, bool ck2, bool ck3, bool ck4)
{
	Mat hsv = Mat(); Mat hue = Mat();
	hue = NULL;
	hsv = NULL;
	cvtColor(input, hsv, COLOR_BGR2GRAY);
	if (ck1 == true)	imshow("GRAY", hsv);
	hue.create(hsv.size(), hsv.depth());
	int ch[] = { 0, 0 };
	mixChannels(&hsv, 1, &hue, 1, ch, 1);
	Mat	a = ClearBlob(hsv, convertSZ(szRectW, szRectH), convertSZ(szKernelW, szKernelH), convertSZ(szBlurW, szBlurH), ck2);
	Mat	b = Thresh_binary(a, convertSZ(szThreshW, szThreshH), ck3);
	Mat c = Canny(b, convertSZ(szClose_x, szClose_y), ck4);
	return c;
}
Mat Filter2(Mat input, int  szRectW, int  szRectH, int szKernelW, int szKernelH, int szBlurW, int szBlurH, int szThreshW, int szThreshH, bool ck1, bool ck2, bool ck3)
{
	Mat hsv = Mat(); Mat hue = Mat();
	hue = NULL;
	hsv = NULL;
	cvtColor(input, hsv, COLOR_BGR2GRAY);
	if (ck1 == true)	imshow("GRAY", hsv);
	hue.create(hsv.size(), hsv.depth());
	int ch[] = { 0, 0 };
	mixChannels(&hsv, 1, &hue, 1, ch, 1);
	Mat	a = ClearBlob(hsv, convertSZ(szRectW, szRectH), convertSZ(szKernelW, szKernelH), convertSZ(szBlurW, szBlurH), ck2);
	Mat	b = Thresh_binary(a, convertSZ(szThreshW, szThreshH), ck3);
	return b;
}
#pragma endregion
#pragma region Tool

Mat FindSampling(String path)
{
	cv::String s = path + "\\sampling\\sampling.png";
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

	drawContours(imgRaw, contoursSampling, 0, cv::Scalar(0, 255, 0), 2, 8, hierarchy, 10);
	//imshow("FindSampling", imgRaw);



	return imgRaw;

}

Mat FindContours(Mat input, Mat raw, double minArea, double maxArea, int szSampling, int xCrop, int yCrop)
{
	vector < string >;
	cv::destroyAllWindows();
	vector<Vec4i> hierarchy;

	cv::Point center;
	cv::Point pMax1;
	cv::Point pMax2;
	cv::Point pFist;
	findContours(input, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	//	imshow("inPut", src);
	vector<Moments> mu(contours.size());
	vector<cv::Point2f> mc(contours.size());

	for (int i = 0; i < contours.size(); i++)
	{
		// cv::drawContours(outPut, contours, i, cv::Scalar(0, 255, 0), 2, 8, hierarchy, 1, cv::Point(xCrop, yCrop));//vẽ biên dạng tìm được

		//Limit Area 
		double area = contourArea(contours[i]);
		if (area <minArea)
			continue;
		if (area >maxArea)
			continue;


		//find point Center
		mu[i] = moments(contours[i]);
		mc[i] = cv::Point2f(static_cast<float>(mu[i].m10 / (mu[i].m00)), static_cast<float>(mu[i].m01 / (mu[i].m00)));
		vector<vector<cv::Point> > hull(contours.size()); // vector<vector<Point> >hull(Contours.size());
		cv::convexHull(cv::Mat(contours[i]), hull[i], true);
		if (hull[i].size() > 2)//nếu biên dạng bao có số điểm lớn hơn 2 thì tiếp tục xử lý
		{
			boundingBox = cv::boundingRect(hull[i]);
			cv::Point offBox = cv::Point(boundingBox.x, boundingBox.y);
			offBox = OffSet(offBox, cv::Point(xCrop, yCrop));
			cv::Rect boundingBox2 = cv::Rect(offBox.x, offBox.y, boundingBox.width, boundingBox.height);
			cv::rectangle(raw, boundingBox2, cv::Scalar(255, 0, 0));
			center = cv::Point(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);
			std::vector<cv::Point> lis_Hull_p;
			getOrientation(contours[i], raw, xCrop, yCrop, i);

			//tim hai diem dau duoi
			double lenMax1 = 2000, lenMax2 = 2000;
			double tLenMax1 = 10000, tLenMax2 = 10000;
			cv::Point pBeginEnd1 = pBE1;
			cv::Point pBeginEnd2 = pBE2;
			int index1 = 0, index2 = 0;
			tLenMax1 = 1000;
			tLenMax2 = 1000;
			lenMax1 = 2000;
			lenMax2 = 2000;
			for (int k = 0; k < contours[i].size(); k++)
			{
				lenMax1 = sqrt(pow(contours[i][k].x - pBeginEnd1.x, 2) + pow(contours[i][k].y - pBeginEnd1.y, 2));
				lenMax2 = sqrt(pow(contours[i][k].x - pBeginEnd2.x, 2) + pow(contours[i][k].y - pBeginEnd2.y, 2));

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
			vector<vector<cv::Point> > contoursUP(contours.size());
			vector<vector<cv::Point> > contoursDown(contours.size());
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
				if (i1 > contours[i].size() - 1)
					i1 = abs(i1 - contours[i].size());
				if (i2 > contours[i].size() - 1)
					i2 = abs(i2 - contours[i].size());
				lenMax1 += sqrt(pow(contours[i][i1].x - contours[i][i2].x, 2) + pow(contours[i][i1].y - contours[i][i2].y, 2));
				indexTren = i1;
				pdau1 = contours[i][i1];
				if (lenMax1 > Sz) break;
			}
			int indexDuoi;	lenMax1 = 0;
			for (int k = index1; true; k--)
			{

				i1 = k;
				i2 = k - 1;
				if (i1 <0)
					i1 = abs(contours[i].size() + i1);
				if (i2 <0)
					i2 = abs(contours[i].size() + i2);

				lenMax1 += sqrt(pow(contours[i][i1].x - contours[i][i2].x, 2) + pow(contours[i][i1].y - contours[i][i2].y, 2));
				indexDuoi = i1;
				pdau2 = contours[i][i1];
				if (lenMax1 > Sz) break;
			}

			for (int k = indexDuoi; true; k++)
			{
				i1 = k;

				if (i1 > contours[i].size() - 1)
					i1 = abs(i1 - contours[i].size());
				contoursUP[i].push_back(contours[i][i1]);
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
				if (i1 > contours[i].size() - 1)
					i1 = abs(i1 - contours[i].size());
				if (i2 > contours[i].size() - 1)
					i2 = abs(i2 - contours[i].size());
				lenMax1 += sqrt(pow(contours[i][i1].x - contours[i][i2].x, 2) + pow(contours[i][i1].y - contours[i][i2].y, 2));
				indexTren = i1;
				pduoi1 = contours[i][i1];
				if (lenMax1 > Sz) break;

			}

			indexDuoi = 0;	lenMax1 = 0;
			for (int k = index2; true; k--)
			{

				i1 = k;
				i2 = k - 1;
				if (i1 <0)
					i1 = abs(contours[i].size() + i1);
				if (i2 <0)
					i2 = abs(contours[i].size() + i2);
				lenMax1 += sqrt(pow(contours[i][i1].x - contours[i][i2].x, 2) + pow(contours[i][i1].y - contours[i][i2].y, 2));
				indexDuoi = i1;
				pduoi2 = contours[i][i1];
				if (lenMax1 > Sz) break;
			}

			for (int k = indexDuoi; true; k++)
			{
				i1 = k;

				if (i1 > contours[i].size() - 1)
					i1 = abs(i1 - contours[i].size());
				contoursDown[i].push_back(contours[i][i1]);

				if (i1 == indexTren) break;
			}

			//So sanh 2 bien dang tim duoc vs bien dang mau
			double area1 = 0; double tArea1 = 0;
			double area2 = 0; double tArea2 = 0;
			cv::Point p1, p2, p3, p4;
			for (int k = 0; k < contoursUP[i].size(); k++)
			{
				if (k == contoursUP[i].size() - 1)
					area1 = sqrt(pow(contoursUP[i][k].x - contoursUP[i][0].x, 2) + pow(contoursUP[i][k].y - contoursUP[i][0].y, 2));
				else
					area1 = sqrt(pow(contoursUP[i][k].x - contoursUP[i][k + 1].x, 2) + pow(contoursUP[i][k].y - contoursUP[i][k + 1].y, 2));

				if (area1>tArea1)
				{
					tArea1 = area1;
					if (k == contoursUP[i].size() - 1)
					{
						p1 = contoursUP[i][k];
						p2 = contoursUP[i][0];
					}
					else
					{
						p1 = contoursUP[i][k];
						p2 = contoursUP[i][k + 1];
					}

				}
			}
			for (int k = 0; k < contoursDown[i].size(); k++)
			{
				if (k == contoursDown[i].size() - 1)
					area2 = sqrt(pow(contoursDown[i][k].x - contoursDown[i][0].x, 2) + pow(contoursDown[i][k].y - contoursDown[i][0].y, 2));
				else
					area2 = sqrt(pow(contoursDown[i][k].x - contoursDown[i][k + 1].x, 2) + pow(contoursDown[i][k].y - contoursDown[i][k + 1].y, 2));

				if (area2>tArea2)
				{
					tArea2 = area2;
					if (k == contoursDown[i].size() - 1)
					{
						p3 = contoursDown[i][k];
						p3 = contoursDown[i][0];
					}
					else
					{
						p3 = contoursDown[i][k];
						p4 = contoursDown[i][k + 1];
					}
				}
			}
			//p1 = contoursUP[i][0];
			//p2 = contoursUP[i][contoursUP.size()-1];
			//p3 = contoursDown[i][0];
			//p4 = contoursDown[i][contoursDown.size() - 1];
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
				match1 = round(cv::matchShapes(contoursUP[i], contoursSampling[0], CV_CONTOURS_MATCH_I1, 0) * 100);
				match2 = round(cv::matchShapes(contoursDown[i], contoursSampling[0], CV_CONTOURS_MATCH_I1, 0) * 100);
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
			//result += to_string(pHut.x) + "," + to_string(pHut.y) + ",0" + "," + to_string(angle) + "\n";
			cv::Point pDraw; pDraw = cv::Point(center.x, center.y);
			cv::Point pDraw2 = cv::Point(pDraw.x + 10, pDraw.y - 10);
			//cv::circle(outPut, pDraw2, 9, cv::Scalar(0, 255, 0), 20);
			//conthu++;
			//	cv::putText(outPut, "1", pDraw, cv::FONT_HERSHEY_COMPLEX_SMALL, 1, cv::Scalar(0, 255, 0), 4); // Anti-alias (Optional)
			//DRAW     // color, 2, 8, hierarchy, 0, Point()
			//cv::drawContours(outPut, contours, i, cv::Scalar(0, 255, 0), 2, 8, hierarchy, 1, cv::Point(xCrop, yCrop));//vẽ biên dạng tìm được

			cv::drawContours(raw, hull, i, cv::Scalar(0, 0, 255), 2, 8, hierarchy, 1, cv::Point(xCrop, yCrop)); //vẽ biên dạng bao
			mc[i] = OffSet(mc[i], cv::Point(xCrop, yCrop));
			//	cv::circle(outPut, mc[i], 2, cv::Scalar(255, 0, 0), 4);//vẽ  vị trí tâm biên dạng tìm được

			//midpoint = OffSet(midpoint, cv::Point(xCrop, yCrop));
			//cv::circle(outPut, midpoint, 2, cv::Scalar(0, 255, 0), 4);
			//curved_point = OffSet(curved_point, cv::Point(xCrop, yCrop));
			//cv::circle(outPut, curved_point, 2, cv::Scalar(0, 255, 0), 4);
			//pHut = OffSet(pHut, cv::Point(xCrop, yCrop));
			//cv::circle(outPut, pHut, 4, cv::Scalar(0, 255, 0), 6);
			if (match1 < match2)
			{
				cv::drawContours(raw, contoursUP, i, cv::Scalar(255, 0, 0), 1, 8, hierarchy, 1, cv::Point(xCrop, yCrop));
				cv::drawContours(raw, contoursDown, i, cv::Scalar(255, 0, 0), 1, 8, hierarchy, 1, cv::Point(xCrop, yCrop));
			}
			else
			{
				cv::drawContours(raw, contoursUP, i, cv::Scalar(255, 0, 0), 1, 8, hierarchy, 1, cv::Point(xCrop, yCrop));
				cv::drawContours(raw, contoursDown, i, cv::Scalar(255, 0, 0), 1, 8, hierarchy, 1, cv::Point(xCrop, yCrop));
			}
			pFist = OffSet(pFist, cv::Point(xCrop, yCrop));
			pDuoi = OffSet(pDuoi, cv::Point(xCrop, yCrop));
			pHut = OffSet(pHut, cv::Point(xCrop, yCrop));
			cv::circle(raw, pHut, 4, cv::Scalar(255, 255, 0), 6);
			//cv::line(outPut, pHut, pFist, cv::Scalar(0, 255, 0), 2);
			//cv::line(outPut, pDuoi, pFist, cv::Scalar(0, 255, 0), 1);
			cv::circle(raw, pFist, 2, cv::Scalar(255, 0, 0), 2);
			cv::circle(raw, pDuoi, 4, cv::Scalar(0, 0, 255), 2);
			drawAxis(raw, pHut, pFist, Scalar(0, 255, 0), 1);
			//	drawAxis(outPut, pHut, pDuoi, Scalar(255, 255, 0),1);

			//imshow("Result", outPut);

		}
	}
	//imshow("Result", outPut);


	return raw;
}

double areaMAX = 0;
bool Trigger(Mat inPut, Mat raw, double minArea)
{
	areaMAX = 0;

	inPut = Mat();
	//cv::destroyAllWindows();
	vector<vector<cv::Point> > contours;
	vector<Vec4i> hierarchy;
	findContours(inPut, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	for (int i = 0; i < contours.size(); i++)
	{
		double  area = contourArea(contours[i]);
		if (area>areaMAX)areaMAX = area;
		if (area < minArea)
			continue;
		else
		{

			return true;

		}
	}
	return  false;
}



#pragma endregion 
#pragma region Process

void fcTrigger(Mat in)
{
	int64 t0 = cv::getTickCount();


	matTrigger = Filter2(in, szRectW, szRectH, szKernelW, szKernelH, szBlurW, szBlurH, szThreshW, szThreshH, ck1, ck2, ck3);
	//Mat  out(matTrigger, cv::Rect(10, 10,
	//	200, 200));
	//areaTriger = countNonZero(in);//cv::Mat::zeros(matTrigger.size(), CV_8UC3);
	areaTriger = countNonZero(matTrigger);
	//cv::parallel_for_(cv::Range(0, 8), Parallel_process(matTrigger, out, 5, 8));

	int64 t1 = cv::getTickCount();
	cycleTrigger = (t1 - t0) / cv::getTickFrequency();

}
void pTrigger(Mat in)
{
	thread process1(fcTrigger, in);
	if (process1.joinable())
	{
		process1.join();
		//cv::destroyAllWindows();
		cycleTrigger = cycleTrigger * 1000;
		cv::imshow("OUT", matTrigger);
		std::cout << "AREA: " << areaTriger << std::endl;
		std::cout << "Cycle time Trigger: " << cycleTrigger << "ms" << std::endl;

	}
}
#pragma endregion
#pragma endregion
std::string s("empty");
void on_mouse_click(int event, int x, int y, int, void*)
{
	if (event == EVENT_LBUTTONDOWN)    //are you missing this?
	{
		std::cout << "enter string: ";
		std::cin >> s;
	}
}
Mat  image;
bool blLoad = false;
VideoCapture cap(1);
int main(int argc, const char *argv[])
{
	cap.set(CV_CAP_PROP_SETTINGS, 1);
	int i = 0;
	/*while (true)
	{
		cap >> frame;

		if (frame.empty())
			break;
		
		
	//	equalizeHist(frame, gray_image);
		cv::imshow("CCD0", frame);
		Mat gray_image;
		cvtColor(frame, gray_image, CV_BGR2GRAY);
		//string path = "pic\\" + to_string(1) + ".png";
		////input = cv::imread(path, COLOR_BGR2GRAY);
		//pTrigger(input);
		//if (i>20)
			//imwrite("save\\" + to_string(i) + ".jpg", gray_image);
		i++;
		cv::waitKey(1);
	}*/
	GetDesktopResolution(with, height);
	const cv::String windows[] = { wMain, wFILTER, WINDOW3_NAME, WINDOW4_NAME };
	moveWindow(wMain, 0, 0);
	image = cv::Mat(height, with, CV_8UC3);
	cvNamedWindow(wMain, CV_WINDOW_NORMAL);
	cvSetWindowProperty(wMain, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	cvui::init(wMain);
	int h = 0;
	int w = 0;
	frCap = cv::Mat(3 * with / 4, height, CV_8UC3);
	while (true) {
		frFilter = cv::Scalar(255, 255, 255);
		if (blRecord == true)
		{
			cap >> frame;
			if (frame.empty())
				break;
			//blSetting = false;
			frame.copyTo(frCap);
		}
		else
		{
			frCap = cv::Scalar(255, 255, 255);
		}
		image = cv::Scalar(255, 255, 255);
		cv::resize(frCap, frCap, cv::Size(3 * with / 4, height), 0, 0, CV_INTER_LINEAR);
		cvui::context(wMain);
		cvui::image(image, 0, 0, frCap);
		fResult(image);
		if (blRecord == true)
		if (cvui::button(image, 3 * with / 8, 9 * height / 10, 80, 30, "Stop")) {
			blRecord = false;
			goto Y;
		}
		if (blRecord == false)
		if (cvui::button(image, 3 * with / 8, 9 * height / 10, 80, 30, "Rec")) {
			blRecord = true;
		}
	Y:if (cvui::button(image, with - 20, 0, 20, 20, "X"))
	{
		  cv::destroyAllWindows();
		  cv::waitKey(1);
		  break;
	}
	  if (cvui::button(image, 110, 0, 100, 30, "OFFLINE")) {
	  }
	  if (blRun == true)
	  if (cvui::button(image, 0, 0, 100, 30, "RUN")) {
		  blRun = false;
		  goto X;
	  }
	  if (blRun == false)
	  if (cvui::button(image, 0, 0, 100, 30, "EDITOR")) {
		  blRun = true;
	  }
  X:
	  if (blRun == false)
	  {
		  if (cvui::button(image, 0, 40, 100, 30, "CCD SETTING")) {
			  cap.set(CV_CAP_PROP_SETTINGS, 1);
		  }
		  if (cvui::button(image, 0, 70, 100, 30, "FILTER")) {
			  blFilter = !blFilter;
			  blTool = false;
		  }
		  if (cvui::button(image, 0, 100, 100, 30, "TOOL")) {
			  blTool = !blTool;
			  blFilter = false;
		  }
		  if (cvui::button(image, 0, 130, 100, 30, "COMMUNICATION"))
		  {
			  blCom = !blCom;
		  }
		  if (cvui::button(image, 0, 160, 100, 30, "RESULT")) {
		  }
		  fTool(image);
		  if (blCom == true)fComunication(image, 100, 130, 300, 180);
		  if (blFilter == true)
		  {
			  //
			  fFilter(image);
			  fSetLocation();
		  }
	  }
	  //cvui::image(image, 50, 50, frFilter);
	  cvui::update(wMain);
	  cv::imshow(wMain, image);
	  cv::waitKey(1);
	}
	/*
 
	for (int i = 1; i < 33; i++)
	{
		string path = "pic\\" + to_string(i) + ".png";
		input = cv::imread(path, COLOR_BGR2GRAY);
		pTrigger(input);
		cv::waitKey(200);
		if (i == 32)i = 0;
	}*/
	cv::waitKey(0);
	/*Result Result();
	
	//cvui::update(wFILTER);
	//cv::imshow(wFILTER, frFilter);
	/*grBlur.begin(frFilter);
	if (!grBlur.isMinimized()) {
	cvui::text("Clear Blob", 0.5, 0x148ae3);
	cvui::checkbox("view", &ck1);//, 0xff0000);
	/*cvui::text(frFilter, 10, 10, "Clear Blob", 0.5, 0x148ae3);
	cvui::checkbox(frFilter, 140, 10, "view", &ck1);//, 0xff0000);
	cvui::text(frFilter, 20, 40, "Size Rec", 0.4, 0x8ccbfb);
	cvui::counter(frFilter, 90, 40, &szRectX,1, "%.1f");
	cvui::counter(frFilter, 90, 65, &szRectY, 1, "%.1f");
	///+60
	cvui::text(frFilter, 20, 100, "Size Kenel", 0.4, 0x8ccbfb);
	cvui::counter(frFilter, 90, 100, &szRectX, 1, "%.1f");
	cvui::counter(frFilter, 90, 125, &szRectY, 1, "%.1f");
	///+60
	cvui::text(frFilter, 20, 160, "Size Blur", 0.4, 0x8ccbfb);
	cvui::counter(frFilter, 90, 160, &szRectX, 1, "%.1f");
	cvui::counter(frFilter, 90, 185, &szRectY, 1, "%.1f");
	grBlur.end();
	//cvui::update(wFILTER);
	}*/

	/*	cvui::beginColumn(frFilter, 20, 240, -1, -1, 6);
	cvui::text(frFilter, 10, 220, "Binary", 0.5, 0x148ae3);
	//cvui::trackbar(width, &doubleValue3, 0., 4., 2, "%.2Lf", cvui::TRACKBAR_DISCRETE, 0.25);
	cvui::trackbar(180, &typeBinary, 1., 4., 2, "%.0Lf", cvui::TRACKBAR_DISCRETE | cvui::TRACKBAR_HIDE_SEGMENT_LABELS, 1.0);
	cvui::endColumn();
	//cvui::trackbar(frFilter, 20, 230, 180, &szRectY, 1., 4., 2, "%.0f", cvui::TRACKBAR_DISCRETE, 1);
	cvui::update(wFILTER);*/
	//cv::imshow(wFILTER, frFilter);

	//fFilter(wFILTER);
	//int w = 1200; int h = 800;
	//cv::resize(image, image, cv::Size(w, h), 0, 0, CV_INTER_LINEAR);
	/*	for (int button = cvui::LEFT_BUTTON; button <= cvui::RIGHT_BUTTON; button++) {
	// Get the anchor, ROI and color associated with the mouse button
	cv::Point& anchor = anchors[button];
	cv::Rect& roi = rois[button];
	unsigned int color = colors[button];
	// The function "bool cvui::mouse(int button, int query)" allows you to query a particular mouse button for events.
	// E.g. cvui::mouse(cvui::RIGHT_BUTTON, cvui::DOWN)
	//
	// Available queries:
	//	- cvui::DOWN: mouse button was pressed. cvui::mouse() returns true for single frame only.
	//	- cvui::UP: mouse button was released. cvui::mouse() returns true for single frame only.
	//	- cvui::CLICK: mouse button was clicked (went down then up, no matter the amount of frames in between). cvui::mouse() returns true for single frame only.
	//	- cvui::IS_DOWN: mouse button is currently pressed. cvui::mouse() returns true for as long as the button is down/pressed.
	// Did the mouse button go down?
	if (cvui::mouse(button, cvui::DOWN)) {
	// Position the anchor at the mouse pointer.
	anchor.x = cvui::mouse().x;
	anchor.y = cvui::mouse().y;
	}
	// Is any mouse button down (pressed)?
	if (cvui::mouse(button, cvui::IS_DOWN)) {
	// Adjust roi dimensions according to mouse pointer
	int width = cvui::mouse().x - anchor.x;
	int height = cvui::mouse().y - anchor.y;
	roi.x = width < 0 ? anchor.x + width : anchor.x;
	roi.y = height < 0 ? anchor.y + height : anchor.y;
	roi.width = std::abs(width);
	roi.height = std::abs(height);
	// Show the roi coordinates and size
	cvui::printf(image, roi.x + 5, roi.y + 5, 0.3, color, "(%d,%d)", roi.x, roi.y);
	cvui::printf(image, cvui::mouse().x + 5, cvui::mouse().y + 5, 0.3, color, "w:%d, h:%d", roi.width, roi.height);
	}
	// Ensure ROI is within bounds
	roi.x = roi.x < 0 ? 0 : roi.x;
	roi.y = roi.y < 0 ? 0 : roi.y;
	roi.width = roi.x + roi.width > image.cols ? roi.width + image.cols - (roi.x + roi.width) : roi.width;
	roi.height = roi.y + roi.height > image.rows ? roi.height + image.rows - (roi.y + roi.height) : roi.height;
	// If the ROI is valid, render it in the frame and show in a window.
	if (roi.area() > 0) {
	cvui::rect(image, roi.x, roi.y, roi.width, roi.height, color);
	cvui::printf(image, roi.x + 5, roi.y - 10, 0.3, color, "Area %d", button);
	cv::imshow("ROI button" + std::to_string(button), frame(roi));
	}
	}
	cvui::update();
	cv::imshow(WINDOW1_NAME, image);
	if (selectObject && selection.width > 0 && selection.height > 0)
	{
	Mat roi(image, selection);
	bitwise_not(roi, roi);
	printf("%d %d %d %d\n", selection.x, selection.y, selection.width, selection.height);
	}

	if (selection.width > 0 && selection.height > 0)
	rectangle(image, selection, cv::Scalar(255, 0, 0));
	// The functions below will update a window and show them using cv::imshow().
	// In that case, you must call the pair cvui::context(NAME)/cvui::update(NAME)
	// to render components and update the window.
	//	window(WINDOW1_NAME);
	//window(WINDOW2_NAME);
	//window(WINDOW3_NAME);
	// The function below will do the same as the funcitons above, however it will
	// use cvui::imshow() (cvui's version of cv::imshow()), which will automatically
	// call cvui::update() for us.
	//compact(WINDOW4_NAME);
	// Check if ESC key was pressed
	char c;
	c = waitKey(30);
	if (c != -1)
	{
	textIntput += c;
	std::cout << textIntput;
	}
	if (cv::waitKey(30) == 27) {
	break;
	}
	//std::cout << s;
	}
	*/
	return 0;
}
