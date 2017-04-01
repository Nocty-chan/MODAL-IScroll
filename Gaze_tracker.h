#ifndef GAZE_TRACKER_H
#define GAZE_TRACKER_H
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include <opencv2/core/utility.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "image.h"
#include "detection.h"
#include <iostream>
#include <ctype.h>

class Gaze_Tracker
{
private: std::string id;
		 Point leftPupil, rightPupil;
		 Rect leftEyebb, rightEyebb;
		 Mat leftEyetpl,rightEyetpl;
		 void trackLeft(Mat & src, Rect & face);
		 void trackRight(Mat & src, Rect & face);
		 Point findDarkerPoint(Mat& src);
		

public:
	Gaze_Tracker(std::string id);
	~Gaze_Tracker();
	void init(const Mat & src, const Rect & bbleft , const Rect & bbright);
	void track(Mat & src, Rect & face);
	bool foundleft, foundright;
	Point getBary(Rect & face);
	
};

#endif