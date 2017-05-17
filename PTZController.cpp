#include "PTZController.h"
#include "http.h"

PTZController::PTZController(const string &IP, int port, const string (&command)[5]) {
	PTZController::IP = IP;
	PTZController::port = port;
	PTZController::command[PTZ_LEFT] = command[PTZ_LEFT];
	PTZController::command[PTZ_RIGHT] = command[PTZ_RIGHT];
	PTZController::command[PTZ_UP] = command[PTZ_UP];
	PTZController::command[PTZ_DOWN] = command[PTZ_DOWN];
	PTZController::command[PTZ_STOP] = command[PTZ_STOP];
}; 


void PTZController::left() {
	try {
		Http http;
		http.connectIP(IP, port);
		http.request(IP, command[PTZ_LEFT], std::cout);
	}
	catch ( const std::exception &e ) {
		cerr << "ERROR: PTZ failed! Is camera connected?" << endl; 
	}
}

void PTZController::right() {
	try {
		Http http;
		http.connectIP(IP, port);
		http.request(IP, command[PTZ_RIGHT], std::cout);
	}
	catch ( const std::exception &e ) {
		cerr << "ERROR: PTZ failed! Is camera connected?" << endl; 
	}
}

void PTZController::up() {
	try {
		Http http;
		http.connectIP(IP, port);
		http.request(IP, command[PTZ_UP], std::cout);
	}
	catch ( const std::exception &e ) {
		cerr << "ERROR: PTZ failed! Is camera connected?" << endl; 
	}
}

void PTZController::down() {
	try {
		Http http;
		http.connectIP(IP, port);
		http.request(IP, command[PTZ_DOWN], std::cout);
	}
	catch ( const std::exception &e ) {
		cerr << "ERROR: PTZ failed! Is camera connected?" << endl; 
	}
}

void PTZController::stop() {
	try {
		Http http;
		http.connectIP(IP, port);
		http.request(IP, command[PTZ_STOP], std::cout);
	}
	catch ( const std::exception &e ) {
		cerr << "ERROR: PTZ failed! Is camera connected?" << endl; 
	}
}

