
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

void Mask_and_Hue(const Mat& src, Mat & mask, Mat & hue);
void Hist(const Rect& face, const Mat & src, Mat & hist);
void Backproj(const Mat & hue, const Mat & mask, const Mat& hist, Mat & backproj);
void Hist_and_Backproj_alt(const Rect& face, const Mat & hue, const Mat & mask, Mat & hist, Mat & backproj);
void TrackFace_alt(Mat & src, Rect & trackWindow, const Mat & hist);
