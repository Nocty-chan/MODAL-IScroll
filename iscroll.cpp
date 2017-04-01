//#include <Windows.h>
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
#include "Gaze_tracker.h"
#include "camshift_tracker.h"
#include <iostream>
#include <ctype.h>

using namespace cv;
using namespace std;

int main(int, char**)
{
	//*********INITIALISATION DES VARIABLES ************************//
	String WELCOME_STRING = "^^^^^^^^^^^^^^^^^^Bienvenue sur IScroll.^^^^^^^^^^^^^^^^^^ \n IScroll va détecter vos yeux et votre visage pour suivre la direction de votre regard sur l'écran. ";
	cout << WELCOME_STRING << endl;
	Mat frame;
	Size oldsize;
	Point bary, center;
	Rect face, lefteye, righteye;
	Tracker trackFace = Tracker("face");
	Gaze_Tracker trackGaze = Gaze_Tracker("Gaze");
	Tracker leftEye = Tracker("left");
	Tracker rightEye = Tracker("right");
	VideoCapture cap(0); // open the default camera
	if (!cap.isOpened())  // check if we succeeded
		return -1;
	namedWindow("Camera", 1);
	bool leftfound = false, rightfound = false;
	cout << "Initialisation, merci de patienter." << endl;

	//***************DETECTION DU VISAGE ET DES YEUX ********************//
	while (true)
	{
		cout << ".";
		cap >> frame;
		imshow("frame", frame);
		char c = (char)waitKey(10);
			findFace(frame, face);

			trackFace.init(frame, face);
			if (waitKey(30) >= 0)
			{
				break;
			}
				if (!leftfound && findLeftEye(frame, face, lefteye) > 0)
				{
					imshow("LEFT EYE", frame(lefteye));
					//leftEye.init(frame, lefteye);
					leftfound = true;

					if (leftfound && rightfound)
					{
						trackGaze.init(frame, lefteye, righteye);
						break;
					}
				}
				if (!rightfound && findRightEye(frame, face, righteye) > 0)
				{
					//cout << "Oeil droit détecté!" << endl;
					imshow("Right EYE", frame(righteye));
					rightfound = true;
					//rightEye.init(frame, righteye);
					if (leftfound && rightfound)
					{
						trackGaze.init(frame, lefteye, righteye);
						break;
					}
				}

		}
	cout << "Initialisation terminée! " << endl;
	cout << "Début du calibrage... regardez devant vous au centre de l'écran et appuyez sur la touche c. " << endl;

	//******************TRACKING DU VISAGE ET DES YEUX: ATTENTE DU SIGNAL DE CALIBRAGE POUR DETERMINER LE POINT DE REFERENCE ***************//
		while (true)
		{
			cap >> frame;
			trackFace.track(frame);
			//rightEye.track(frame);
			//leftEye.track(frame);
			trackGaze.track(frame, trackFace.trackWindow);
			//resize(frame, frame,frame.size() * 2);

				bary = trackGaze.getBary(trackFace.trackWindow);
				center = Point(trackFace.trackWindow.x + trackFace.trackWindow.width / 2, trackFace.trackWindow.y + trackFace.trackWindow.height / 2);
				circle(frame, center, 3, Scalar(0, 125, 10));
				circle(frame, bary+Point(trackFace.trackWindow.x,trackFace.trackWindow.y), 5, Scalar(135, 0, 0));
				imshow("cap", frame);

			if (waitKey(30) =='c')
				{
					oldsize = trackFace.trackWindow.size();
					break;
				}

		}
		cout << "Calibrage terminé... " << endl;

		Mat game = imread("../image.jpg", CV_LOAD_IMAGE_COLOR);
		Rect zone;
		Rect limits = Rect(0, 0, game.cols, game.rows);
		int x = 125, y = 125;
		int threshx = 5;
		int threshy = 2;
		zone = Rect(Point(x, y), Size(1000, 1000))&limits;
		imshow("game", game(zone));
		//***********CONNAISSANT LE POINT DE REFERENCE: TRACKING DU VISAGE ET DES YEUX, TRACKING DU REGARD**************//
		while (true)
		{

			cap >> frame;
			trackFace.track(frame);
			trackGaze.track(frame, trackFace.trackWindow);

			Point temp = trackGaze.getBary(trackFace.trackWindow);
			center = Point(trackFace.trackWindow.x + trackFace.trackWindow.width / 2, trackFace.trackWindow.y + trackFace.trackWindow.height / 2);
			circle(frame, center, 3, Scalar(0, 125, 10));
			circle(frame, bary + Point(trackFace.trackWindow.x/**trackFace.trackWindow.size().width / oldsize.width*/, trackFace.trackWindow.y/**trackFace.trackWindow.size().height/oldsize.height*/), 5, Scalar(135, 0, 0));
			circle(frame, temp + Point(trackFace.trackWindow.x, trackFace.trackWindow.y), 5, Scalar(135, 0, 150));
			imshow("cap", frame);
			int step = 100;
			if (trackGaze.foundleft && trackGaze.foundright)
			{
				int dx = abs(temp.x - bary.x);
				int dy = abs(temp.y - bary.y);
				if (temp.x - bary.x <= -threshx && dx>=dy)
				{
					x = min(x + step, game.cols-step);
					zone = Rect(Point(x, y), Size(1000, 1000))&limits;
					cout << "Moved to the right" << endl;
					cout << "X: " << x << " Y: " << y << endl;
				}
				if (temp.x - bary.x >= threshx && dx>=dy )
				{

					x = max(x - step, step);
					zone = Rect(Point(x, y), Size(1000, 1000))&limits;
					cout << "Moved to the left" << endl;
					cout << "X: " << x << " Y: " << y << endl;
				}
				if (temp.y - bary.y <= -threshy && dy>=dx)
				{
					y = max(y - step, step);
					zone = Rect(Point(x, y), Size(1000, 1000))&limits;
					cout << "Moved to the top" << endl;
					cout << "X: " << x << " Y: " << y << endl;
				}
				if (temp.y - bary.y >= threshy && dy>=dx)
				{
					zone = Rect(Point(x, y), Size(1000, 1000))&limits;
					zone &= limits;
					cout << "Moved to the bottom" << endl;
					cout << "X: " << x << " Y: " << y << endl;
				}
				imshow("game", game(zone));
			}

			if (waitKey(30) == 'c')
			{
				cout << "RECALIBRATING" << endl;
				//Recalibrate
				bary = trackGaze.getBary(trackFace.trackWindow);
				circle(frame, bary + Point(trackFace.trackWindow.x, trackFace.trackWindow.y), 5, Scalar(135, 0, 0));
				oldsize = trackFace.trackWindow.size();
			}
			if (waitKey(30) == 'd')
			{
				cout << "STOP" << endl;
				break;
			}
		}
	}
