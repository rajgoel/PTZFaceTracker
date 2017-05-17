#include <string> /* string */
#include <sstream> /* ostream, stringstream */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <iostream> /* cout, cerr, endl */
#include <limits.h> /* UINT_MAX */

using namespace std;

class Http {
public:
	Http(); // connects to IP if given, or looks up IP from URL
	~Http();
	void connectIP(const string &IP, const int &port=80);
	void connectURL(const string &URL, const int &port=80);
	void setEndOfStream(string pattern = "</html>", unsigned int length = UINT_MAX) { endOfStream = pattern; maxStreamLength = length;};
	void request(const string &hostname, const string &path, ostream &response=cout);
private:
	int sockfd;
	struct sockaddr_in serv_addr;
	unsigned int maxStreamLength;
	string endOfStream;
};


