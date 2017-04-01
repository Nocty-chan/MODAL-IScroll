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

#include <iostream>
#include <ctype.h>


void Mask_and_Hue(const Mat& src, Mat & mask, Mat & hue);

void Hist_and_Backproj(Mat& src, MatND & hist, MatND& backproj);

void TrackFace(Mat & src, Rect & face, Rect & trackWindow);
