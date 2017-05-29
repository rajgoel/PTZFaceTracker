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

#ifdef LOG
 #include <iomanip>      // std::put_time
 #include <ctime>        // std::time_t, struct std::tm, std::localtime
 #include <chrono>       // std::chrono::system_clock
 #include <fstream>       // std::fstream
#endif


using namespace std;

class FaceTracker
{
public:
	FaceTracker(string source);
	~FaceTracker();
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

#ifdef LOG
	chrono::time_point<chrono::system_clock> start;
	chrono::milliseconds duration;
	string filename;
	cv::VideoWriter video;
	ofstream* logfile;
#endif

};

