#include "opencv2/highgui.hpp" /* cv::waitKey */
#include <iostream>
#include <fstream>

#include "FaceTracker.h"
#include "PTZController.h"
#include "http.h"

#include <string>
using namespace std;

int main(int argc, char** argv)
{
	if ( argc < 3 ) {
		cerr << "Usage: " << argv[0] << " <video device> <config file>" << endl;
		exit(0);
	}

	int device = atoi(argv[1]);
	string IP;
	int port;
	string command[5];
	string param, value;

	ifstream *config = new ifstream(argv[2]);
	if ( config->is_open()) {
		while (!config->eof()) {
			*config >> param;
			*config >> value;
			if ( param == "IP" ) {
				IP = value;
			}
			else if ( param == "PORT" ) {
				port = atoi(value.c_str());
			}
			else if ( param == "PTZ_LEFT" ) {
				command[PTZController::PTZ_LEFT] = value;
			}
			else if ( param == "PTZ_RIGHT" ) {
				command[PTZController::PTZ_RIGHT] = value;
			}
			else if ( param == "PTZ_UP" ) {
				command[PTZController::PTZ_UP] = value;
			}
			else if ( param == "PTZ_DOWN" ) {
				command[PTZController::PTZ_DOWN] = value;
			}
			else if ( param == "PTZ_STOP" ) {
				command[PTZController::PTZ_STOP] = value;
			}
		}
		config->close();
	}
	else {
		cerr << "Cannot open config file!" << endl;
	}
	delete config;

	try {
		cout << "Try to connect to camera ..." << endl;
		Http http;
		http.connectIP(IP, port);
		http.request(IP, command[PTZController::PTZ_STOP], std::cout);
	}
	catch ( const std::exception &e ) {
		cerr << "ERROR: Cannot connect to camera!" << endl;
		exit(0);
	}

	FaceTracker faceTracker = FaceTracker(device);
	int width, height;
	faceTracker.getDimensions(width, height);

	PTZController ptz = PTZController(IP, port, command);

	char key;
	unsigned int PTZ_TRACK = PTZController::PTZ_STOP;
	unsigned int PTZ_MANUAL = PTZController::PTZ_STOP;
	do
	{
		if ( faceTracker.update() ) {
			int x, y;
			faceTracker.getPosition(x, y);
			
			
			if ( PTZ_MANUAL == PTZController::PTZ_STOP ) {
				// Camera is not manuallly controlled

				// Stop PTZ if position is close to center
				if ( PTZ_TRACK == PTZController::PTZ_LEFT && x >= 0.3 * width ) {
					ptz.stop();
					PTZ_TRACK = PTZController::PTZ_STOP;
					faceTracker.setLabel("");
				}
				else if ( PTZ_TRACK == PTZController::PTZ_RIGHT && x <= 0.7 * width ) {
					ptz.stop();
					PTZ_TRACK = PTZController::PTZ_STOP;
					faceTracker.setLabel("");
				}
				else if ( PTZ_TRACK == PTZController::PTZ_UP && y >= 0.4 * height ) {
					ptz.stop();
					PTZ_TRACK = PTZController::PTZ_STOP;
					faceTracker.setLabel("");
				}
				else if ( PTZ_TRACK == PTZController::PTZ_DOWN && y <= 0.6 * height ) {
					ptz.stop();
					PTZ_TRACK = PTZController::PTZ_STOP;
					faceTracker.setLabel("");
				}

				// Start PTZ if position is close to border

				if ( x < 0.2 * width ) {
					ptz.left();
					PTZ_TRACK = PTZController::PTZ_LEFT;
					faceTracker.setLabel("Automatic move: LEFT");
				}
				else if  ( x > 0.8 * width ) {
					ptz.right();
					PTZ_TRACK = PTZController::PTZ_RIGHT;
					faceTracker.setLabel("Automatic move: RIGHT");
				}
				else if  ( y < 0.3 * height ) {
					ptz.up();
					PTZ_TRACK = PTZController::PTZ_UP;
					faceTracker.setLabel("Automatic move: UP");
				}
				else if  ( y > 0.7 * height ) {
					ptz.down();
					PTZ_TRACK = PTZController::PTZ_DOWN;
					faceTracker.setLabel("Automatic move: DOWN");
				}
			}
		}
		else {
			if ( PTZ_TRACK != PTZController::PTZ_STOP ) {
				// Stop automatic PTZ if lost
				ptz.stop();
				PTZ_TRACK = PTZController::PTZ_STOP;
				faceTracker.setLabel("");
			}
		}

		key = cv::waitKey(25); // this is also needed to display videos
		switch (key) {
		   case 81:   // do something
//std::cout << "LEFT" << std::endl;
			ptz.left();
			PTZ_MANUAL = PTZController::PTZ_LEFT;
			PTZ_TRACK = PTZController::PTZ_STOP;
			faceTracker.setLabel("Manual move: LEFT");
			break;
		   case 82:   // do something
//std::cout << "UP" << std::endl;
			ptz.up();
			PTZ_MANUAL = PTZController::PTZ_UP;
			PTZ_TRACK = PTZController::PTZ_STOP;
			faceTracker.setLabel("Manual move: UP");
			break;
		   case 83:   // do something else
//std::cout << "RIGHT" << std::endl;
			ptz.right();
			PTZ_MANUAL = PTZController::PTZ_RIGHT;
			PTZ_TRACK = PTZController::PTZ_STOP;
			faceTracker.setLabel("Manual move: RIGHT");
			break;
		   case 84:   // do something
//std::cout << "DOWN" << std::endl;
			ptz.down();
			PTZ_MANUAL = PTZController::PTZ_DOWN;
			PTZ_TRACK = PTZController::PTZ_STOP;
			faceTracker.setLabel("Manual move: DOWN");
			break;
		   case 32:   // SPACE
//std::cout << "SPACE" << std::endl;
			ptz.stop();
			faceTracker.next();
			PTZ_MANUAL = PTZController::PTZ_STOP;
			PTZ_TRACK = PTZController::PTZ_STOP;
			faceTracker.setLabel("");
			break;
		   case 10:   // ENTER
//std::cout << "ENTER" << std::endl;
			ptz.stop();
			faceTracker.hookup();
			PTZ_MANUAL = PTZController::PTZ_STOP;
			PTZ_TRACK = PTZController::PTZ_STOP;
			faceTracker.setLabel("");
			break;
		   default:    // unhandled key
//if ( key >= 0) std::cout << "Key code: " << (int)key << std::endl;
			break;
		}
	} while ( key != 27 ); // Loop until 'ESC' is pressed 

	return 0;
}
