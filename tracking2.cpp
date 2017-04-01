#include "tracking2.h"

void Hist(const Rect& face, const Mat & src, Mat & hist)
{
	Mat mask, hue;
	int hsize = 16;
	float hranges[] = { 0, 180 };
	Mask_and_Hue(src, mask, hue);
	const float* phranges = hranges;
	Mat roi(hue, face);
	Mat maskroi(mask, face);
	calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
	normalize(hist, hist, 0, 255, NORM_MINMAX);
	show_hist(hist);
}
void Backproj(const Mat & hue, const Mat & mask, const Mat& hist, Mat & backproj)
{
	float hranges[] = { 0, 180 };
	const float* phranges = hranges;
	calcBackProject(&hue, 1, 0, hist, backproj, &phranges);
	backproj &= mask;
	imshow("BACKPROJ", backproj);
}
void Hist_and_Backproj_alt(const Rect& face, const Mat & hue, const Mat & mask, Mat & hist, Mat & backproj)
{
	int hsize = 16;
	float hranges[] = { 0, 180 };
	const float* phranges = hranges;
	Mat roi(hue, face);
	Mat maskroi(mask, face);
	calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
	normalize(hist, hist, 0, 255, NORM_MINMAX);
	show_hist(hist);

}


void TrackFace_alt(Mat & src, Rect & trackWindow,const Mat & hist)
{
	Mat mask, hue,backproj;
	Mask_and_Hue(src, mask, hue);
	Backproj(hue, mask, hist, backproj);
	RotatedRect trackBox = CamShift(backproj, trackWindow,
		TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 10, 1));
	if (trackWindow.area() <= 1)
	{
	int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5) / 6;
	trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
	trackWindow.x + r, trackWindow.y + r) &
	Rect(0, 0, cols, rows);
	}

	ellipse(src, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
	rectangle(src, trackWindow, Scalar(255, 0, 0));
	rectangle(src, trackBox.boundingRect(), Scalar(255, 0, 0));
}