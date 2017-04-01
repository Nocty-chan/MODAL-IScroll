#ifndef TRACKER_H
#define TRACKER_H
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

class Tracker
{
private: Rect ooi;
		 std::string id; 
		 Mat roi, initframe, hist1, hist2, backproj;
		 int hbins =6, sbins = 4, thresh = 0;
		 // we compute the histogram from the 0-th and 1-st channels
		 void show_hist2();
		 void show_hist1();
		 void show_hist();
		 void onerangehistogram();
		 void tworangehistogram();
		 void thresholdproj();
		 void contourproj();
		 void getbackproj(const Mat & src);
		

public:
	Rect trackWindow;
	Tracker(std::string id);
	~Tracker();
	bool init(const Mat & src, const Rect & tracked);
	void track( Mat & src);
};

#endif