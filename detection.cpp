#include "detection.h"


double dist(const Rect & rect1, const Rect & rect2)
{
	return
		sqrt((rect1.x + rect1.width / 2 - rect2.x - rect2.width / 2)*(rect1.x + rect1.width / 2 - rect2.x - rect2.width / 2) +
		(rect1.y + rect1.height / 2 - rect2.y - rect2.height / 2)*(rect1.y + rect1.height / 2 - rect2.y - rect2.height / 2));
}
void drawFace1(Mat& src)
{
	CascadeClassifier face_cascade_alt;
	face_cascade_alt.load("C:/opencv/sources/data/haarcascades/haarcascade_frontalface_alt.xml");
	if (face_cascade_alt.empty())
	{
		cout << "couldn't load Cascade Classifier" << endl;
		return;
	}
	Mat gray;
	vector<Rect> faces_alt;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	face_cascade_alt.detectMultiScale(gray, faces_alt);

	for (int i = 0; i < faces_alt.size(); i++)
	{
		rectangle(src, faces_alt[i], CV_RGB(255, 255, 0));
	}

}
void drawFace2(Mat& src)
{
	CascadeClassifier face_cascade_alt2;
	face_cascade_alt2.load("C:/opencv/sources/data/haarcascades/haarcascade_frontalface_alt2.xml");
	if (face_cascade_alt2.empty())
	{
		cout << "couldn't load Cascade Classifier" << endl;
		return;
	}
	Mat gray;
	vector<Rect> faces_alt2;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	face_cascade_alt2.detectMultiScale(gray, faces_alt2);

	for (int i = 0; i < faces_alt2.size(); i++)
	{
		rectangle(src, faces_alt2[i], CV_RGB(255, 255, 255));
	}

}
void findFace(Mat& src, Rect& face)
{
	CascadeClassifier face_cascade_alt2;
	face_cascade_alt2.load("C:/opencv/sources/data/haarcascades/haarcascade_frontalface_alt2.xml");
	if (face_cascade_alt2.empty())
	{
		cout << "couldn't load Cascade Classifier" << endl;
		return;
	}
	Mat gray;
	vector<Rect> faces_alt2;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	face_cascade_alt2.detectMultiScale(gray, faces_alt2);
	if (faces_alt2.size() > 0)
	{
		face = faces_alt2[0] &= Rect(0, 0, src.cols, src.rows);
		return;
	}
	else face = Rect(0, 0, src.cols, src.rows);
}
void drawCircles(Mat& src)
{
	Mat gray;
	vector<Vec3f> circles;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	gray.convertTo(gray, CV_8U);
	//GaussianBlur(gray, gray, Size(9, 9), 2, 2);
	HoughCircles(gray, circles, CV_HOUGH_GRADIENT, 1, 1);
	for (int i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
		int radius = cvRound(circles[i][2]);
		// draw the circle center
		circle(src, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		// draw the circle outline
		circle(src, center, radius, Scalar(0, 0, 255), 3, 8, 0);
	}
}
void drawEyes(Mat& src)
{
	CascadeClassifier eye_cascade_right, eye_cascade_left;
	eye_cascade_left.load("C:/opencv/sources/data/haarcascades/haarcascade_lefteye_2splits.xml");
	eye_cascade_right.load("C:/opencv/sources/data/haarcascades/haarcascade_righteye_2splits.xml");
	if (eye_cascade_right.empty())
	{
		cout << "couldn't load Cascade Classifier" << endl;
		return;
	}
	Mat gray;
	vector<Rect>righteyes, lefteyes;
	cvtColor(src, gray, COLOR_BGR2GRAY);
	eye_cascade_right.detectMultiScale(gray, righteyes);
	for (int i = 0; i < righteyes.size(); i++)
	{
		rectangle(src, righteyes[i], CV_RGB(0, 0, 255));
	}

	eye_cascade_left.detectMultiScale(gray, lefteyes);
	for (int i = 0; i < lefteyes.size(); i++)
	{
		rectangle(src, lefteyes[i], CV_RGB(0, 255, 255));
	}

}
int findLeftEye(Mat& src, const Rect & face, Rect & leftEye)
{
	CascadeClassifier eye_cascade_left;
	eye_cascade_left.load("C:/opencv/sources/data/haarcascades/haarcascade_lefteye_2splits.xml");
	if (eye_cascade_left.empty())
	{
		cout << "couldn't load Cascade Classifier" << endl;
		return -1;
	}
	Mat gray;
	vector<Rect> lefteyes;
	Rect leftarea = Rect(face.x+face.width/2, face.y, face.width*0.5, face.height*0.75);
	leftarea &= Rect(0, 0, src.cols, src.rows);
	cvtColor(src(leftarea), gray, COLOR_BGR2GRAY);
	eye_cascade_left.detectMultiScale(gray, lefteyes, 1.1, 2,
		CV_HAAR_SCALE_IMAGE, cv::Size(20, 20));
	imshow("LEFTAREA", gray);
	if (lefteyes.size() > 0)
	{
		Rect lefteye_cropped = Rect(lefteyes[0].x, lefteyes[0].y+lefteyes[0].height*0.4, lefteyes[0].width*0.8, lefteyes[0].height*0.5);
		leftEye =lefteye_cropped+ cv::Point(leftarea.x, leftarea.y);
		leftEye = leftEye &= Rect(0, 0, src.cols, src.rows);
		return 1;
	}
	
	else return -1;
}


int findRightEye(Mat& src, const Rect & face,Rect & rightEye)
{

	CascadeClassifier eye_cascade_right;
	eye_cascade_right.load("C:/opencv/sources/data/haarcascades/haarcascade_righteye_2splits.xml");
	if (eye_cascade_right.empty())
	{
		cout << "couldn't load Cascade Classifier" << endl;
		return -1;
	}
	Mat gray;
	vector<Rect> righteyes;
	Rect rightarea = Rect(face.x, face.y, face.width*0.5, face.height*0.6);
	rightarea &= cv::Rect(0, 0, src.cols, src.rows);
	cvtColor(src(rightarea), gray, COLOR_BGR2GRAY);
	imshow("RIGHTAREA", gray);
	eye_cascade_right.detectMultiScale(gray, righteyes, 1.1, 2,
		CV_HAAR_SCALE_IMAGE, cv::Size(20, 20));
	if (righteyes.size() > 0)
	{
		Rect righteye_cropped =Rect(righteyes[0].x, righteyes[0].y+righteyes[0].height*0.4, righteyes[0].width*0.8, righteyes[0].height*0.5);
		rightEye = righteye_cropped + cv::Point(rightarea.x, face.y);
		rightEye = rightEye &= Rect(0, 0, src.cols, src.rows);
		return 1;
	}
	else return -1;
}
void DrawRotatedRect(RotatedRect & rrect, Mat & src)
{
	Point2f vertices[4];
	rrect.points(vertices);
	for (int i = 0; i < 4; i++)
		line(src, vertices[i], vertices[(i + 1) % 4], Scalar(0, 255, 0));
}

void show_hist(const Mat &hist, std::string & id)
{
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
	imshow("HIST "+id, histimg);
}