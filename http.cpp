#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include <arpa/inet.h> /* inet_pton */
#include <stdexcept> /* runtime_error */

#include "http.h"
#define READBUF 4096

Http::Http() {
	maxStreamLength = UINT_MAX;
	endOfStream = "</html>";
	
	/* create the socket */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) throw runtime_error("Cannot open socket");
}

Http::~Http() {
	/* close the socket */
	close( sockfd );
}

void Http::connectIP(const string &IP, const int &port) {
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	if ( inet_pton(AF_INET, IP.c_str() , &serv_addr.sin_addr.s_addr) == 0 ) {
		throw runtime_error("Invalid IP address");
	}
	/* connect the socket */
	if ( connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(sockaddr_in) ) < 0) {
		throw runtime_error("Failed to connect to server");
	}
}

void Http::connectURL(const string &URL, const int &port) {
	/* lookup the ip address */
	struct hostent *server;
	server = gethostbyname(URL.c_str());
	if (server == NULL) {
		throw runtime_error("Host not found");
	}

	/* fill in the structure */
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);
	memcpy(&serv_addr.sin_addr.s_addr,server->h_addr,server->h_length);

	/* connect the socket */
	if ( connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(sockaddr_in)) < 0 ) {
		throw runtime_error("Failed to connect to server");
	}
}


void Http::request(const string &hostname, const string &path, ostream &response) {
	string message = "GET " + path + " HTTP/1.1\r\nHost: " + hostname + "\r\n\r\n";
	/* send the request */
	if ( write(sockfd,message.c_str(),message.size()) < 0) {
		throw runtime_error("Cannot write message to socket");
	}

	/* receive the response */
	char buffer[READBUF];
	int iReadBytes = 0;
	std::string read;
	do {
		iReadBytes = recv(sockfd,buffer,sizeof(buffer),MSG_NOSIGNAL);
		if (iReadBytes > 0) {
			read.append(buffer, iReadBytes);
		}
		if ( read.size() >= maxStreamLength || ( endOfStream.size() && read.find(endOfStream) != std::string::npos ) ) {
			break;
		}
	} while (iReadBytes > 0);
 
	response << read;
}


