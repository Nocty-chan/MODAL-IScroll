#include "camshift_tracker.h"


Tracker::Tracker(std::string identifiant)
{
	id = identifiant; 
}

Tracker::~Tracker()
{

}
bool Tracker::init(const Mat & src, const Rect & tracked)
{
	src.copyTo(initframe);
	ooi = tracked;
	roi = initframe(tracked);
	trackWindow = ooi;
	tworangehistogram();
	if (ooi.area()==src.cols*src.rows)return false;
	return true;
}
void Tracker::show_hist()
{
	Mat hist = hist2;
	int hsize = 16;
	Mat histimg = Mat::zeros(200, 320, CV_8UC3);
	histimg = Scalar::all(0);
	int binW = histimg.cols / hsize;
	Mat buf(1, hsize, CV_8UC3);
	for (int i = 0; i < hsize; i++)
		buf.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i*180. / hsize), 255, 255);
	cvtColor(buf, buf, COLOR_HSV2BGR);
	for (int i = 0; i < hsize; i++)
	{
		int val = saturate_cast<int>(hist.at<float>(i)*histimg.rows / 255);
		rectangle(histimg, Point(i*binW, histimg.rows),
			Point((i + 1)*binW, histimg.rows - val),
			Scalar(buf.at<Vec3b>(i)), -1, 8);
	}
	imshow("HIST " + id, histimg);
}
void Tracker::show_hist2()
{
	Mat hist = hist2;
	double maxVal = 0;
	minMaxLoc(hist, 0, &maxVal, 0, 0);

	int scale = 10;
	Mat histImg = Mat::zeros(sbins*scale, hbins * 10, CV_8UC3);

	for (int h = 0; h < hbins; h++)
		for (int s = 0; s < sbins; s++)
		{
			float binVal = hist.at<float>(h, s);
			int intensity = cvRound(binVal * 255 / maxVal);
			rectangle(histImg, Point(h*scale, s*scale),
				Point((h + 1)*scale - 1, (s + 1)*scale - 1),
				Scalar::all(intensity),
				CV_FILLED);
		}
	namedWindow("H-S Histogram" +id, 1);
	imshow("H-S Histogram" +id, histImg);
}
void Tracker::tworangehistogram()
{
	int channels[] = { 0, 1 };
	Mat hsv;
	cvtColor(roi, hsv, COLOR_BGR2HSV);
	imshow("hsv " + id, hsv);
	int histSize[] = { hbins, sbins };
	// hue varies from 0 to 179, see cvtColor
	float hranges[] = { 0, 180 };
	// saturation varies from 0 (black-gray-white) to
	// 255 (pure spectrum color)
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	
	calcHist(&hsv, 1, channels, Mat(), // do not use mask
		hist2, 2, histSize, ranges);
	normalize(hist2, hist2, 0, 255, NORM_MINMAX);
	show_hist();
}

void Tracker::onerangehistogram()
{
	Mat hsv;
	cvtColor(roi, hsv, COLOR_BGR2HSV);
	int hsize = hbins;
	imshow("hsv " + id, hsv);
	// Quantize the hue to 30 levels
	// and the saturation to 32 levels
	float hranges[] = { 0, 180 };
	const float* phranges = { hranges};
	// we compute the histogram from the 0-th and 1-st channels
	calcHist(&hsv, 1, { 0 }, Mat(), hist1, 1, &hsize, &phranges); //PROBLEME SUR CALCHIST
	normalize(hist1, hist1, 0, 255, NORM_MINMAX);
	show_hist();
}

void Tracker::getbackproj(const Mat & src){
	float hranges[] = { 0, 180 };
	// saturation varies from 0 (black-gray-white) to
	// 255 (pure spectrum color)
	float sranges[] = { 0, 256 };
	const float* ranges[] = { hranges, sranges };
	int channels[] = { 0, 1 };
	Mat frame,hsv;
	src.copyTo(frame);
	medianBlur(frame, frame, 11);
	imshow("blrred", frame);
	createTrackbar("hbin", "backproj "+id, &hbins, 14, 0);
	createTrackbar("sbin", "backproj " + id, &sbins, 16, 0);
	createTrackbar("threshold", "backproj " + id, &thresh, 255, 0);
	cvtColor(frame,hsv, CV_BGR2HSV);
	calcBackProject(&hsv,1,channels, hist2, backproj, ranges);
	imshow("beforebackproj " + id, backproj);
	thresholdproj();
	imshow("backproj " + id, backproj);
	//contourproj();
	//imshow("contourbackproj " + id, backproj);
}
void Tracker::thresholdproj()
{
	threshold(backproj, backproj, thresh, 0, THRESH_TOZERO);
	threshold(backproj, backproj, 125, 255, THRESH_BINARY);
}
void Tracker::contourproj()
{
	RNG rng(12345);
	Mat copy;
	medianBlur(backproj, backproj, 3);
	Canny(backproj,backproj, thresh, thresh * 2, 3);
	vector<vector<Point> > contours;
	vector<vector<Point> > contours_filtered;
	vector<Vec4i> hierarchy;
	findContours(backproj, contours, hierarchy, CV_RETR_LIST,CV_CHAIN_APPROX_NONE);
	for (int i = 0; i < contours.size(); i++)
	{
		vector<Point> contour = contours[i];
		if (contourArea(contour)>30)
		{
			contours_filtered.push_back(contour);
		}

	}
	drawContours(backproj, contours_filtered, -1,Scalar(0,0,0));
}

void Tracker::track(Mat & src)
{
	
	getbackproj(src);
	RotatedRect tracked = CamShift(backproj, trackWindow, TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
	trackWindow = tracked.boundingRect();
	cv::Rect tempface;
	if (0 <= trackWindow.x && 0 <= trackWindow.width && trackWindow.x + trackWindow.width <= src.cols &&
		0 <= trackWindow.y && 0 <= trackWindow.height && trackWindow.y + trackWindow.height <= src.rows)
	{
		trackWindow &= cv::Rect(0, 0, src.cols, src.rows);
		imshow("trackWindow", src(trackWindow));
		//findFace(src(trackWindow),tempface);
		Mat trackWindoMat = src(trackWindow);
		rectangle(trackWindoMat, tempface, Scalar(123, 12, 255));
	}

	ellipse(src, tracked, Scalar(0, 123, 12));
}
