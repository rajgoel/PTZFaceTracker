#include "FaceTracker.h"
#include <stdexcept> /* runtime_error */
#include <math.h> /* sqrt, pow */


FaceTracker::FaceTracker() {
	window_name = "Face Tracker";
	if ( !face_cascade.load("haarcascade_frontalface_alt.xml") ) {
		throw runtime_error("Cannot load cascade file!");
	}
	LOST = true;
	MOTION = false;
	selected = 0;
}

FaceTracker::FaceTracker(int device) {
	*this = FaceTracker();
	camera = cv::VideoCapture(device);
	if (!camera.isOpened()) {
		throw runtime_error("Cannot open camera");
	}
	camera >> frame;
	width = frame.cols;
	height = frame.rows;
	target.x = width/2;
	target.y = height/2;
}

FaceTracker::FaceTracker(string video) {
	*this = FaceTracker();
	camera = cv::VideoCapture(video);
	if (!camera.isOpened()) {
		throw runtime_error("Cannot open video");
	}
	camera >> frame;
	width = frame.cols;
	height = frame.rows;
	target.x = width/2;
	target.y = height/2;
}

void FaceTracker::getDimensions(int &width, int &height) {
	width = FaceTracker::width;
	height = FaceTracker::height;
}

double FaceTracker::dist(int x1, int y1, int x2, int y2) {
	return sqrt( pow(x1 - x2,2) + pow(y1 - y2,2) );
}

void FaceTracker::getPosition(int &x, int &y) {
	x = position.x + position.width/2;
	y = position.y + position.height/2;
}

void FaceTracker::setLabel(const string &label) {
	FaceTracker::label = label;
}

void FaceTracker::startMotion() {
	MOTION = true;
}

void FaceTracker::stopMotion() {
	MOTION = false;
	// re-initialize the tracker to the last match
	tracker = cv::Tracker::create( "KCF" );
	tracker->init(frame,position);
//	LOST = false;
}

bool FaceTracker::update() {
	camera >> frame;

	cv::Mat frame_gray;

	// Convert to gray scale
	cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);

	// Equalize histogram
	cv::equalizeHist(frame_gray, frame_gray);

	// Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.3, 5);

	double best = faces.size() ? dist(faces[0].x + faces[0].width/2, faces[0].y + faces[0].height/2, target.x, target.y) : -1;
	selected = 0;
	// Determine face closest to target
	for( size_t i = 1; i < faces.size(); i++ ) {
		double d = dist(faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2, target.x, target.y);
		if ( d < best ) {
			best = d;
			selected = i;
		}
	}

	if ( faces.size() && LOST ) {
		hookup();
	}

	if ( !LOST ) {
		if ( MOTION && lastMatch.cols > 0 && lastMatch.rows > 0) {
			// use last match to update position
			cv::Mat result;
			cv::matchTemplate(frame, lastMatch, result, CV_TM_SQDIFF_NORMED);
// cv::normalize( result, result, 0, 1, NORM_MINMAX, -1, Mat() );

			/// localize the best match with minMaxLoc
			double minVal, maxVal; 
			cv::Point minLoc, maxLoc;
			cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
			// update tracking position
    			position = cv::Rect2d(minLoc.x, minLoc.y, lastMatch.cols, lastMatch.rows);

			// update last match if full match is visible
			if ( position.x >= 0 && position.x + position.width <= frame.cols && position.y >= 0 && position.y + position.height <= frame.rows ) {
				lastMatch = frame(position).clone();
			}

			// show matched face
			cv::rectangle( frame, position, cv::Scalar( 255, 255, 0 ), 2, 1 );
		}
		else {
			// update the tracking result
			tracker->update(frame,position);
			// update last match if full match is visible
			if ( position.x >= 0 && position.x + position.width <= frame.cols && position.y >= 0 && position.y + position.height <= frame.rows ) {
				lastMatch = frame(position).clone();
			}

			// draw the tracked object
			if ( position.x + position.width > 0 && position.x < frame.cols && position.y + position.height > 0 && position.y < frame.rows ) {
				// tracked object is still on the video
				cv::rectangle( frame, position, cv::Scalar( 255, 0, 0 ), 2, 1 );
			}
			else {
				LOST = true;
			}
		}
	}

	// Iterate over all of the faces
	for( size_t i = 0; i < faces.size(); i++ ) {
		// Find center of face
		cv::Point center(faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2);
		// Draw green ellipse around selected face and red ellipse around all other faces
		ellipse(frame, center, cv::Size(faces[i].width/2, faces[i].height/2), 0, 0, 360, (i == selected) ? cv::Scalar( 0, 255, 0 ) : cv::Scalar( 0, 0, 255 ), 4, 8, 0 );
	}

	cv::Point pos(0, 20);
	putText(frame, label, pos, cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(255, 255, 255), 1); 

	// Display frame
	cv::imshow( window_name, frame );

	if ( lastMatch.cols > 0 && lastMatch.rows > 0 ) cv::imshow( "Template", lastMatch );

	return !LOST;
}

void FaceTracker::next() {
	if ( faces.size() > 1 ) {
		// select next face and update target position
		selected++;
		if ( selected >= faces.size() ) selected = 0;
		target.x = faces[selected].x + faces[selected].width/2;
		target.y = faces[selected].y + faces[selected].height/2;
	}

}

void FaceTracker::hookup() {
	if ( selected < faces.size() ) {
		// initialize the tracker to the selected face
		tracker = cv::Tracker::create( "KCF" );
		tracker->init(frame,faces[selected]);
		LOST = false;
	}
}
