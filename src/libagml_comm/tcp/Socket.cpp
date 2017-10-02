/*
Copyright © CNRS 2015. 
Authors: Jerôme Fellus, David Picard and Philippe-Henri Gosselin
Contact: jerome.fellus@ensea.fr, picard@ensea.fr, gosselin@ensea

This software is governed by the CeCILL license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.

*/

#include "Socket.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include <sys/poll.h>
#include "../util/utils.h"
#include <stdexcept>
#include <signal.h>
#include <netdb.h>
#include <ifaddrs.h>


///////////
// DEBUG //
///////////

#define SOCKET_DBG_READ(socket, buffer, n) do {  \
	__dbg_append_toFile((socket), "r : "); \
    if((n)==sizeof(int)) {__dbg_append_toFile((socket), "%i", *(int*) (buffer) ); } \
    else { __dbg_append_toFile((socket), (char*) (buffer), (n)); } \
    __dbg_append_toFile((socket), "\n") } while(false)

#define SOCKET_DBG_WRITE(socket, buffer, n) do { \
	__dbg_append_toFile(socket, "w : "); \
    if(n==sizeof(int)) {__dbg_append_toFile(socket, "%i", *(int*)buffer); } \
    else { __dbg_append_toFile(socket, (char*)buffer, n); } \
    __dbg_append_toFile(socket, "\n"); } while(false)


static bool _bInited = false;
static void _init_() {
	if(_bInited) return;
	signal(SIGPIPE, SIG_IGN);
	_bInited = true;
}


Socket::Socket(const char* ip, unsigned short int port) {
	if(!_bInited) _init_();
	readBlocking = true; readTimeout = 0;
	bConnected = false;

	strcpy(this->ip, ip);
	this->port = port;
	this->bClient = true;
	connect();
}


Socket::Socket(const char* url, bool multiple_attempts) {
	if(!_bInited) _init_();
	readBlocking = true; readTimeout = 0;
	bConnected = false;
	bClient = true;
	socket = 0;

	// Parse URL
	const char* sp;
	if((sp = strchr(url, ':'))) {
		port = atoi(sp+1);
		strncpy(this->ip, url, sp-url);
		ip[sp-url] = 0;
	} else {
		strcpy(this->ip, url);
		port = DEFAULT_PORT;
	}

	connect(multiple_attempts ? 10 : 1);
}




void Socket::connect(int nb_attempts) {
	if(!_bInited) _init_();
	bClient = true;

	// Create socket
    socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (socket < 0) throw("can't create socket");

	int flag = 1;
	setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));

	for(int attempt = 1; attempt<=nb_attempts; attempt++) {
		try {
			// Create address
			struct hostent *server;
			if (!(server = gethostbyname(this->ip))) throw std::runtime_error(TOSTRING("No such host : " << ip << ":" << port));
			struct sockaddr_in serv_addr;
			bzero((char *) &serv_addr, sizeof(serv_addr));
			serv_addr.sin_family = AF_INET;
			bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
			serv_addr.sin_port = htons(port);

			// Connect socket to addr
			if (::connect(socket,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
				throw std::runtime_error(TOSTRING("Connection to " << ip << ":" << port << " refused"));
			break;
		} catch(std::exception& e) {
			if(attempt==nb_attempts) throw std::runtime_error(e.what());
			else {DBG(e.what() << " (attempt " << attempt << ")"); sleep(1);}
		}
	}

    bConnected = true;
}

Socket::Socket(int socket, struct sockaddr_in* addr, size_t len) {
	if(!_bInited) _init_();
	readBlocking = true; readTimeout = 0;
	int flag = 1;
	setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));
	this->socket = socket;
	strcpy(this->ip, inet_ntoa(addr->sin_addr));
	this->port = ntohs(addr->sin_port);

	bClient = false;
	bConnected = true;
}

Socket::~Socket() {
	shutdown(socket, SHUT_RDWR);
	close(socket);
}


void Socket::write(void* buffer, size_t size) {
    int n = ::write(socket,buffer,size);
    if (n < 0) throw SocketClosedException();

#ifdef SOCKET_DEBUG
    SOCKET_DBG_WRITE(socket, buffer, n);
#endif
}


bool Socket::waitForMsg(int timeout_ms) {
	struct pollfd pfd;
	pfd.fd = socket; pfd.events = POLLIN;
	poll(&pfd, 1, timeout_ms);
	return pfd.revents & POLLIN;
}

size_t Socket::read(void* buffer, size_t maxSize) {
	int n;
	if(!readBlocking && readTimeout>0) if(!waitForMsg(this->readTimeout)) throw "Timeout exceeded";
	n = ::recv(socket,buffer,maxSize, readBlocking ? 0 : MSG_DONTWAIT);
	if (n < 0) throw std::runtime_error("ERROR reading from socket");
	if (n == 0) throw SocketClosedException();

#ifdef SOCKET_DEBUG
		SOCKET_DBG_READ(socket, buffer, n);
#endif
    return n;
}

size_t Socket::read_exactly(void* buffer, size_t maxSize) {
	char* buf = (char*)buffer;
	size_t ntot = 0;
	while(ntot < maxSize) {
		int n;
		if(!readBlocking && readTimeout>0) if(!waitForMsg(this->readTimeout)) throw std::runtime_error("Timeout exceeded");
		n = ::recv(socket,buf,maxSize-ntot, readBlocking ? 0 : MSG_DONTWAIT);
		if (n < 0) throw std::runtime_error("ERROR reading from socket");
		if (n == 0) throw SocketClosedException();
		ntot+=n;
		buf+=n;
	}

#ifdef SOCKET_DEBUG
		SOCKET_DBG_READ(socket, buffer, n);
#endif
    return ntot;
}

void Socket::writeFile(const char* filename) {
	struct stat stat_buf;
	int f = open(filename, O_RDONLY);
		fstat(f, &stat_buf);
		write(&(stat_buf.st_size), sizeof(stat_buf.st_size));
		sendfile(socket, f, NULL, stat_buf.st_size);
	close(f);
	usleep(20000);
}

void Socket::readFile(const char* out_filename) {
	char buf[1024];
	size_t n;
	read(&n, sizeof(n));

	DBG("File is of size " << n);

	FILE* f = fopen(out_filename, "w");

	while(n>0) {
		size_t nn = read(buf, sizeof(buf));
		n -= nn;
		fwrite(buf,1,nn,f);
		DBG(n << " remaining");
	}

	fclose(f);
}

bool Socket::readAck() {
	char ack[256];
	try{read(ack, 256);}catch(const char* s) {ERROR(s << " ACK reception timeout"); throw s;}
	return !strcmp(ack, "OK");
}


std::string resolve_ip(const std::string& hostname) {
	std::string ip = hostname;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in *h;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;

    if ( (rv = getaddrinfo( hostname.c_str() , "http" , &hints , &servinfo)) != 0)
    {
//        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return ip;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        h = (struct sockaddr_in *) p->ai_addr;
        ip = inet_ntoa( h->sin_addr );
    }

    freeaddrinfo(servinfo); // all done with this structure

    return ip;
}

/** @return the first valid IP address of this machine's network interfaces */
std::string get_my_first_ip() {
	std::string ip;
	struct ifaddrs *ifaddr, *ifa;
	int family;
	char host[NI_MAXHOST];
	if (getifaddrs(&ifaddr) == -1) return "";

	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL) continue;
		family = ifa->ifa_addr->sa_family;
		if(family != AF_INET) continue; // Only resolve IPV4 addresses
		if(getnameinfo(ifa->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST)) continue;
		ip = host;
		if(ip!="127.0.0.1")	break;
	}

	freeifaddrs(ifaddr);
	return ip;
}
