#include "tracking1.h"
void Hist_and_Backproj(Mat& src, MatND & hist, MatND& backproj)
{
	Mat hsv, hue;
	int bins = 25;
	int histSize = MAX(bins, 2);
	float hue_range[] = { 0, 180 };
	const float* ranges = { hue_range };

	/// Transform it to HSV
	cvtColor(src, hsv, CV_BGR2HSV);

	/// Use only the Hue value
	hue.create(hsv.size(), hsv.depth());
	int ch[] = { 0, 0 };
	mixChannels(&hsv, 1, &hue, 1, ch, 1);

	/// Get the Histogram and normalize it

	calcHist(&hue, 1, 0, Mat(), hist, 1, &histSize, &ranges, true, false);
	normalize(hist, hist, 0, 255, CV_MINMAX);

	/// Get Backprojection

	calcBackProject(&hue, 1, 0, hist, backproj, &ranges, 1, true);

	/// Draw the backproj
	imshow("BackProj", backproj);

	/// Draw the histogram
	int w = 400; int h = 400;
	int bin_w = cvRound((double)w / histSize);
	Mat histImg = Mat::zeros(w, h, CV_8UC3);

	for (int i = 0; i < bins; i++)
	{
		rectangle(histImg, Point(i*bin_w, h), Point((i + 1)*bin_w, h - cvRound(hist.at<float>(i)*h / 255.0)), Scalar(0, 0, 255), -1);
	}

	imshow("Histogram", histImg);

}
void TrackFace(Mat & src, Rect & face, Rect & trackWindow)
{
	Mat hist, backproj, mask, hue;
	Mask_and_Hue(src, mask, hue);
	Hist_and_Backproj(src, hist, backproj);
	RotatedRect trackBox = CamShift(backproj, trackWindow,
		TermCriteria(TermCriteria::EPS | TermCriteria::COUNT, 30, 78));
	//trackWindow = trackBox.boundingRect();
	if (trackWindow.area() <= 1)
	{
		int cols = backproj.cols, rows = backproj.rows, r = (MIN(cols, rows) + 5) / 6;
		trackWindow = Rect(trackWindow.x - r, trackWindow.y - r,
			trackWindow.x + r, trackWindow.y + r) &
			Rect(0, 0, cols, rows);
	}
	ellipse(src, trackBox, Scalar(0, 0, 255), 3, LINE_AA);
	rectangle(src, trackBox.boundingRect(), Scalar(255, 0, 0));
}
