#include <string> /* string */

using namespace std;

class PTZController {
public:
	PTZController(const string &IP, int port, const string (&command)[5]); 
	~PTZController() {}; 
	enum Controls {PTZ_LEFT, PTZ_RIGHT, PTZ_UP, PTZ_DOWN, PTZ_STOP};

	void left();
	void right();
	void up();
	void down();
	void stop();
private:
	string IP;
	int port;
	string command[5];
};


