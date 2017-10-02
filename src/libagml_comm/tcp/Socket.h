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

#ifndef AGML_TCP_SOCKET_H_
#define AGML_TCP_SOCKET_H_

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <exception>
#include <string>
#include <stdexcept>

#define DEFAULT_PORT 10001
//#define SOCKET_DEBUG // Uncomment for socket messages debug


#define SOCKET_WRITE_STRING_DELAY 10000


class SocketClosedException : public std::runtime_error {
public:
	SocketClosedException() : std::runtime_error("SocketClosedException") {}
};


class Socket {
public:
	int socket;
private:
	bool bConnected;
	bool readBlocking;
	int readTimeout;
	bool bClient;
public:
	char ip[256];
	unsigned short int port;
public:
	Socket(const char* ip, bool multiple_attempts = false);
	Socket(const char* ip, unsigned short int port);
	Socket(int socket, struct sockaddr_in* addr, size_t len);
	virtual ~Socket();

	void setReadBlocking(bool rb) {this->readBlocking = rb;}
	void setReadTimeout(size_t timeout) {setReadBlocking(false); readTimeout = timeout;}

	bool isConnected() {return bConnected;}
	bool isClient() {return bClient;}
	bool isServer() {return !bClient;}

	bool waitForMsg(int timeout_ms = -1);
	bool hasMsg() {return waitForMsg(0);}

	void write(void* buffer, size_t size);
	size_t read(void* buffer, size_t maxSize);
	size_t read_exactly(void* buffer, size_t maxSize);


	inline size_t read(long *x) { return read_exactly(x, sizeof(long)); }
	inline size_t read(size_t *x) { return read_exactly(x, sizeof(size_t)); }
	inline size_t read(float *x) { return read_exactly(x, sizeof(float)); }
	inline size_t read(double *x) { return read_exactly(x, sizeof(double)); }
	inline size_t read(int *x) { return read_exactly(x, sizeof(int)); }
	inline size_t read(unsigned int *x) { return read_exactly(x, sizeof(unsigned int)); }
	inline size_t read(unsigned char *x) { return read_exactly(x, sizeof(unsigned char)); }
	inline size_t read(char *x) { return read_exactly(x, sizeof(char)); }
	inline size_t read(unsigned short *x) { return read_exactly(x, sizeof(unsigned short)); }

	inline size_t readString(char* x) {	size_t l; read(&l); read_exactly(x,l); return l;}
	inline std::string readString() { size_t l; read(&l); char* s = new char[l]; read_exactly(s,l); std::string ss; ss = s; delete[] s; return ss;}

	inline void write(long x) { write(&x, sizeof(long)); }
	inline void write(size_t x) { write(&x, sizeof(size_t)); }
	inline void write(float x) { write(&x, sizeof(float)); }
	inline void write(double x) { write(&x, sizeof(double)); }
	inline void write(int x) { write(&x, sizeof(int)); }
	inline void write(unsigned int x) { write(&x, sizeof(unsigned int)); }
	inline void write(unsigned char x) { write(&x, sizeof(unsigned char)); }
	inline void write(char x) { write(&x, sizeof(char)); }
	inline void write(unsigned short x) { write(&x, sizeof(unsigned short)); }
	inline void write(const std::string& s) { write((void*)s.c_str(), s.length()+1); }

	inline void writeString(const char* str) {	size_t l = strlen(str)+1; write(l);	write((void*)str, l); }
	inline void writeString(const std::string& str) {	writeString(str.c_str()); }

	void writeFile(const char* filename);
	void readFile(const char* out_filename);

	bool readAck();

	void dump() {
		printf("%s:%i,%s", ip, port, readBlocking ? "block" : (readTimeout<0 ? "non-block" : "timeout"));
		if(!readBlocking && readTimeout>0) printf("(%ums)",readTimeout);
	}
protected:
	void connect(int nb_attempts = 1);
};


/** @return the IP address of the given network hostname */
std::string resolve_ip(const std::string& hostname);

/** @return the first valid IP address of this machine's network interfaces */
std::string get_my_first_ip();

#endif /* AGML_TCP_SOCKET_H_ */
