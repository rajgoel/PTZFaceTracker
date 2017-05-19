#pragma once

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>

#include <string> /* string */
#include <list> /* list */
#include <iostream> /* cout, cerr, endl */


using namespace std;

class FaceTracker
{
public:
	FaceTracker(int device);
	FaceTracker(string video);
	bool update();
	void getDimensions(int &width, int &height);
	void getPosition(int &x, int &y);
	void next();
	void hookup();
	void setLabel(const string &label);
	void startMotion();
	void stopMotion();
private:
	FaceTracker();
	cv::VideoCapture camera;
	int width;
	int height;
	cv::Mat frame;
	cv::String window_name;
	string label;
	cv::CascadeClassifier face_cascade;
	vector<cv::Rect> faces;
	unsigned int selected;

	bool LOST;
	cv::Ptr<cv::Tracker> tracker;
	cv::Rect2d position;
	cv::Point target;
	double dist(int x1, int y1, int x2, int y2);

	bool MOTION;
	cv::Mat firstMatch;
	cv::Mat lastMatch;
};

