#include"Gaze_tracker.h"
#include <iostream>


Gaze_Tracker::Gaze_Tracker(string identifiant)
{
	id = identifiant;
}
Gaze_Tracker::~Gaze_Tracker()
{

}
int d = 10;

void Gaze_Tracker::init(const Mat & src, const Rect & bbleft, const Rect & bbright)
{
	Mat gray;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	Rect face;
	leftEyebb = bbleft;
	rightEyebb = bbright;
	leftEyetpl = gray(leftEyebb);
	rightEyetpl = gray(rightEyebb);
	foundright =true;
	foundleft = true;
}

/**
* Perform template matching to search the user's eye in the given image.
*
* @param   im    The source image
* @param   tpl   The eye template
* @param   rect  The eye bounding box, will be updated with _
*                the new location of the eye
*/
int dim =10;
void Gaze_Tracker::trackLeft(Mat & src, Rect & face)
{
	if (foundleft)
	{
		int seuil = face.height / dim;
		Rect rect = leftEyebb;
		Mat im;
		cvtColor(src, im, COLOR_BGR2GRAY);
		Mat tpl = leftEyetpl;
		//----FINDING NEW POSITION OF THE LEFT EYE-----------
		cv::Size size(rect.width * 2, rect.height * 2);
		Rect window = (Rect(face.x + face.width*0.5, face.y + face.height*0.2, face.width*0.5, face.height*0.5));
		window &= cv::Rect(0, 0, im.cols, im.rows);
		imshow("Searchzoneleft", src(window));
		cv::Mat dst(window.width - tpl.rows + 1, window.height - tpl.cols + 1, CV_32FC1);
		cv::matchTemplate(im(window), tpl, dst, CV_TM_SQDIFF_NORMED);

		double minval, maxval;
		cv::Point minloc, maxloc;
		cv::minMaxLoc(dst, &minval, &maxval, &minloc, &maxloc);

		if (minval <= 0.2)
		{
			rect.x = window.x + minloc.x;
			rect.y = window.y + minloc.y;
		}
		else
			rect.x = rect.y = rect.width = rect.height = 0;

		//-------CORRECTION -----------//

		if (rect.area() >0 && abs(rect.y - leftEyebb.y)<seuil)
		{
			leftEyebb = rect;
		}
		else if (rect.area() == 0)
		{
			cout << "Left eye lost" << endl;
			foundleft = false;
		}
		else
		{
			cout << "Left eye moved too much" << endl;
			foundleft = false;
			//leftEyebb = rect &= Rect(0, 0, im.cols, im.rows);
		}
	}
	else
	{
		foundleft = findLeftEye(src, face, leftEyebb) == 1;
		if (foundleft)
		{
			cout << "Left eye refound" << endl;
			imshow("LEFT EYE", src(leftEyebb));
			init(src, leftEyebb, rightEyebb);
			//foundleft = false;
		}
	}

}
void Gaze_Tracker::trackRight(Mat & src, Rect & face)
{
	if (foundright)
	{
		int seuil = face.height / dim;
		Rect rect = rightEyebb;
		Mat im;
		cvtColor(src, im, COLOR_BGR2GRAY);
		Mat tpl = rightEyetpl;

		cv::Size size(rect.width * 2, rect.height * 2);
		Rect window = (Rect(face.x, face.y + face.height*0.2, face.width*0.5, face.height*0.5));
		window &= cv::Rect(0, 0, im.cols, im.rows);
		imshow("searchzoneright", src(window));
		Mat dst(window.width - tpl.rows + 1, window.height - tpl.cols + 1, CV_32FC1);
		try{
			matchTemplate(im(window), tpl, dst, CV_TM_SQDIFF_NORMED);
		}
		catch (const std::exception & Exp){

		}

		double minval, maxval;
		Point minloc, maxloc;
		minMaxLoc(dst, &minval, &maxval, &minloc, &maxloc);

		if (minval <= 0.2)
		{
			rect.x = window.x + minloc.x;
			rect.y = window.y + minloc.y;
		}
		else
			rect.x = rect.y = rect.width = rect.height = 0;

		//-------CORRECTION -----------//

		if (rect.area() >0&& abs(rect.y - rightEyebb.y)<seuil)
		{
			rightEyebb = rect &= Rect(0, 0, im.cols, im.rows);
		}
		else if (rect.area() == 0)
		{
			foundright = false;
			cout << "Right eye lost" << endl;
		}
		else
		{
			cout << "Right eye moved too much" << endl;
			foundright = false;
			//rightEyebb = rect &= Rect(0, 0, im.cols, im.rows);
		}
	}
	else
	{
		foundright = (findRightEye(src, face, rightEyebb) == 1);
		if (foundright)
		{
			cout << "Right eye refound" << endl;
			imshow("RIGHT EYE", src(rightEyebb));
			init(src, leftEyebb, rightEyebb);
			//foundright = false;
		}
	}

}

Point Gaze_Tracker::findDarkerPoint(Mat& src){
	double minVal, maxVal;
	Point minLoc, maxLoc;
	Mat gray;
	cvtColor(src, gray, CV_BGR2GRAY);
	minMaxLoc(gray, &minVal, &maxVal, &minLoc, &maxLoc);
	int minValue = gray.at<uchar>(minLoc);
	//circle(src, minLoc, 3, CV_RGB(255, 0, 255), 2);
	//starts friends
	vector<Point> pupille;
	for (int x = 0; x < src.rows; x++){
		for (int y = 0; y < src.cols; y++){
			if (gray.at<uchar>(x, y) <= minValue + d && gray.at<uchar>(x, y) >= minValue - d){
				Point temp = Point(y, x);
				pupille.push_back(temp);
				//circle(src, temp, 3, CV_RGB(255, 255, 255), 2);
			}
		}
	}
	if (pupille.empty()){
		return Point(0, 0);
	}
	int xbary = 0; int ybary = 0;
	for (int i = 0; i < pupille.size(); i++){
		xbary += pupille[i].x;
		ybary += pupille[i].y;
	}
	xbary = (int)xbary / pupille.size();
	ybary = (int)ybary / pupille.size();
	Point bary = Point(xbary, ybary);
	circle(src, bary, 1, CV_RGB(0, 255, 255), 1);
	return bary;
}

void Gaze_Tracker::track(Mat &src, Rect & face )
{
	trackRight(src,face);
	trackLeft(src,face);
	rectangle(src,rightEyebb , Scalar(0, 123, 12));
	rectangle(src, leftEyebb, Scalar(0, 123, 12));
	Rect rightEye_corrected = rightEyebb;// Rect(rightEyebb.x, rightEyebb.y + rightEyebb.height*0.4, rightEyebb.width*0.8, rightEyebb.height*0.5);
	Rect leftEye_corrected = leftEyebb;//Rect(leftEyebb.x, leftEyebb.y + leftEyebb.height*0.4, leftEyebb.width*0.8, leftEyebb.height*0.5);
	Mat rightEyeCorrectedMat = src(rightEye_corrected);
	Mat leftEyeCorrectedMat = src(leftEye_corrected);
	rightPupil = findDarkerPoint(rightEyeCorrectedMat) + Point(rightEye_corrected.x, rightEye_corrected.y);
	leftPupil = findDarkerPoint(leftEyeCorrectedMat) + Point(leftEye_corrected.x, leftEye_corrected.y);
}

Point Gaze_Tracker::getBary(Rect & face)
{
	Point bary = (rightPupil + leftPupil) / 2- Point(face.x,face.y);
	return bary ;
}