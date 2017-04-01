#include <opencv2/highgui/highgui.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iostream>
#include<ctype.h>

using namespace cv;
using namespace std;

void drawFace1(Mat& src);
void drawFace2(Mat& src);
void findFace(Mat& src, Rect& face);
int findLeftEye(Mat & src, const Rect & face, Rect& leftEye);
int findRightEye(Mat & src, const Rect & face, Rect& rightEye);
void drawCircles(Mat& src);
void drawEyes(Mat& src);
void DrawRotatedRect(RotatedRect & rrect, Mat & src);
void show_hist(const Mat &hist);
double dist(const Rect & rect1, const Rect & rect2);