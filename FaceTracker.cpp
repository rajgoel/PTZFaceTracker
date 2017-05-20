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

FaceTracker::FaceTracker(string video) {
	*this = FaceTracker();
	if ( to_string( atoi( video.c_str() ) ) == video ) {
		// if argument is a device number
		camera = cv::VideoCapture( atoi( video.c_str() ) );
	}
	else {
		camera = cv::VideoCapture( video );
	}	
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
	cv::Mat display;
	copyMakeBorder( frame, display, 0, 60, 0, 0, cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0) );

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

	bool MATCH = false;
	if ( faces.size() && firstMatch.cols > 0 && firstMatch.rows > 0 ) {
		// try to focus on face matching the first face
		for( size_t i = 0; i < faces.size(); i++ ) {
			cv::Mat result;
			cv::matchTemplate(frame(faces[i]), firstMatch, result, cv::TM_CCOEFF_NORMED);
			double minVal, maxVal; 
			cv::Point minLoc, maxLoc;
			cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
			if ( maxVal > 0.5 ) {
				// re-initialize the tracker to the matching face
				tracker = cv::Tracker::create( "KCF" );
				tracker->init(frame,faces[i]);

				// update tracking position
	    			position = cv::Rect2d(faces[i]);

				// update last match if full match is visible
				if ( position.x >= 0 && position.x + position.width <= frame.cols && position.y >= 0 && position.y + position.height <= frame.rows ) {
					lastMatch = frame(position).clone();
				}
				MATCH = true;
				LOST = false;
				// show matched face
				cv::rectangle( display, position, cv::Scalar( 255, 255, 0 ), 2, 1 );
			}
		}
	}
/*
	if ( faces.size() && !MATCH && LOST ) {
		// automatically hookup to selected face if lost
		hookup();
	}
*/
	if ( !MATCH && LOST ) {
		if ( lastMatch.cols > 0 && lastMatch.rows > 0) {
			// use last match to find face again
			cv::Mat result;
			cv::matchTemplate(frame, lastMatch, result, cv::TM_CCOEFF_NORMED);

			/// localize the best match with minMaxLoc
			double minVal, maxVal; 
			cv::Point minLoc, maxLoc;
			cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

			if ( maxVal > 0.75 ) {
				// update tracking position if match is good
   				position = cv::Rect2d(maxLoc.x, maxLoc.y, lastMatch.cols, lastMatch.rows);
			
				// update last match if match is inside frame
				if ( position.x > 0 && position.x + position.width < frame.cols && position.y > 0 && position.y + position.height < frame.rows ) {
					lastMatch = frame(position).clone();
					// show matched face
					cv::rectangle( display, position, cv::Scalar( 255, 127, 0 ), 2, 1 );
					// initialize the tracker to the matching face
					tracker = cv::Tracker::create( "KCF" );
					tracker->init(frame,position);
					LOST = false; 
				}
			}
		}
	}
	else if ( !MATCH && !LOST ) {
		if ( MOTION && lastMatch.cols > 0 && lastMatch.rows > 0) {
			// use last match to update position
			cv::Mat result;
			cv::matchTemplate(frame, lastMatch, result, cv::TM_CCOEFF_NORMED);

			/// localize the best match with minMaxLoc
			double minVal, maxVal; 
			cv::Point minLoc, maxLoc;
			cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

			if ( maxVal > 0.75 ) {
				// update tracking position if match is good
   				position = cv::Rect2d(maxLoc.x, maxLoc.y, lastMatch.cols, lastMatch.rows);
			
				// update last match if match is inside frame
				if ( position.x > 0 && position.x + position.width < frame.cols && position.y > 0 && position.y + position.height < frame.rows ) {
					lastMatch = frame(position).clone();
					// show matched face
					cv::rectangle( display, position, cv::Scalar( 255, 127, 0 ), 2, 1 );
					// initialize the tracker to the matching face
					tracker = cv::Tracker::create( "KCF" );
					tracker->init(frame,position);
				}
			} 
			else { 
				LOST = true; 
			}
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
				cv::rectangle( display, position, cv::Scalar( 255, 0, 0 ), 2, 1 );
			}
			else {
				LOST = true;
			}
		}
	}

	for( size_t i = 0; i < faces.size(); i++ ) {
		// Find center of face
		cv::Point center(faces[i].x + faces[i].width/2, faces[i].y + faces[i].height/2);
		// Draw green ellipse around selected face and red ellipse around all other faces
		ellipse(display, center, cv::Size(faces[i].width/2, faces[i].height/2), 0, 0, 360, (i == selected) ? cv::Scalar( 0, 255, 0 ) : cv::Scalar( 0, 0, 255 ), 4, 8, 0 );
	}


	// Display frame
	if ( firstMatch.cols > 0 && firstMatch.rows > 0) {
		cv::Mat inlay;
		cv::resize(firstMatch, inlay, cv::Size( (int)(60), (int)(60) ) );
		inlay.copyTo(display(cv::Rect(width-60, height, 60, 60)));
	}
	if ( lastMatch.cols > 0 && lastMatch.rows > 0) {
		cv::Mat inlay;
		cv::resize(lastMatch, inlay, cv::Size( (int)(60), (int)(60) ) );
		inlay.copyTo(display(cv::Rect(width-120, height, 60, 60)));
	}

	cv::Point pos(30, height + 36);
	putText(display, label, pos, cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(255, 255, 255), 1); 
	cv::imshow( window_name, display );

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
		// update last match if full match is visible
		if ( faces[selected].x >= 0 && faces[selected].x + faces[selected].width <= frame.cols && faces[selected].y >= 0 && faces[selected].y + faces[selected].height <= frame.rows ) {
			cv::Rect face = cv::Rect(faces[selected].x + faces[selected].width/6, faces[selected].y + faces[selected].height/6, faces[selected].width/1.5, faces[selected].height/1.5 );
			firstMatch = frame(face).clone();
		}
		// initialize the tracker to the selected face
		tracker = cv::Tracker::create( "KCF" );
		tracker->init(frame,faces[selected]);
		LOST = false;
	}
}
