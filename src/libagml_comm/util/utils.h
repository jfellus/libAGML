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

#ifndef UTILS_H_
#define UTILS_H_


#include "string.h"
#include "file.h"
#include "func_ptr.h"
#include <pthread.h>
#include <string>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <list>
#include <stdexcept>

#define INTERNAL

class DebugStream {
public:
	std::ostream& stream;
public:
	DebugStream(std::ostream& stream) : stream(stream) {}
	virtual ~DebugStream(){}
	virtual void end() {}
};
class DebugStreamStd : public DebugStream {
public:
	DebugStreamStd() : DebugStream(std::cerr) {}
	virtual ~DebugStreamStd() {}
};
extern DebugStream* DEBUG_STREAM;
extern DebugStream* ERROR_STREAM;

#define FAIL(msg) do { fprintf (stderr, "FAIL: %s\n", msg); exit (-1); } while (0)

#define DBG(x) do {DEBUG_STREAM->stream << x << "\n"; DEBUG_STREAM->end();} while(0)
#define DBGX(x) std::cerr << x << "\n"
#define DBGV(x) DBG("" #x " = " << x)

#define PAUSE(x) do { DBG(x); sleep(1); } while(0)

#define ERROR(x)  do {ERROR_STREAM->stream << "\x1b[31m" << x << "\x1b[0m\n"; ERROR_STREAM->end();} while(0)
#include <algorithm>

template <class T> bool vector_remove(std::vector<T>& v, T val) {
	typename std::vector<T>::iterator position = std::find(v.begin(), v.end(), val);
	if (position != v.end()) {v.erase(position); return true;}
	return false;
}


template <class T> std::ostream& operator<<(std::ostream& os, const std::vector<T*>& v) {
	os << "[ ";
	for(uint i=0; i<v.size(); i++) {
		if(i!=0) os << ", ";
		os << v[i];
	}
	os << " ]";
	return os;
}

template <class T> std::ostream& operator<<(std::ostream& os, const std::vector<T>& v) {
	os << "[ ";
	for(uint i=0; i<v.size(); i++) {
		if(i!=0) os << ", ";
		os << v[i];
	}
	os << " ]";
	return os;
}

template <class T> std::ostream& operator<<(std::ostream& os, const std::list<T>& v) {
	os << "[ ";
	for(typename std::list<T>::const_iterator i=v.begin(); i!=v.end(); i++) {
		if(i!=v.begin()) os << ", ";
		os << *i;
	}
	os << " ]";
	return os;
}

template <typename T> void _VECTOR_ASSERT_UNICITY(const std::vector<T>& v, T item) {
	for(uint i=0; i<v.size(); i++) {
		if(v[i]==item) {
			throw "Not unique";
		}
	}
}

#ifdef DEBUG
#define VECTOR_ASSERT_UNICITY(vector, item) do {try {_VECTOR_ASSERT_UNICITY(vector, item); } catch(const char* s) {DBG("ERROR : not unique ! " << item << " in " << __FILE__ << ":" << __LINE__);}} while(0)
#else
#define VECTOR_ASSERT_UNICITY(vector, item)
#endif


inline int shell(const std::string& command) {return system(command.c_str());}


#ifndef MAX
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

void PRINT_STACK_TRACE();
std::string GET_STACK_TRACE();

std::string get_cur_thread_name();


void TIC();
void TAC();

long get_time_seconds();
long get_time_ms();

std::string str_date();


#define __UNIQUE_VAR(x,ch) __##x##ch
#define _UNIQUE_VAR(x,ch) __UNIQUE_VAR(x,ch)
#define UNIQUE_VAR(x) _UNIQUE_VAR(x, __LINE__ )


void* shared_mem(const std::string& path, size_t size = (size_t)-1);



/**
 * If val is NULL, write zeros with same size as val
 */
template <typename T> void ptrstream_write(unsigned char*& p, T* val, bool bWrite = false) {
	T* pp = (T*)p;
	if(bWrite) {
		if(val) memcpy(p, val, sizeof(*pp));
		else memset(p, 0, sizeof(*pp));
	}
	p += sizeof(*pp);
}

inline void ptrstream_write(unsigned char*& p, const std::string& s, bool bWrite = false) {
	size_t l = s.length();
	ptrstream_write(p, &l, bWrite);
	if(bWrite) memcpy(p, s.c_str(), l);
	p += l;
}

template <typename T> T ptrstream_read(const unsigned char*& p) {
	T* pp = (T*)p;
	p += sizeof(T);
	return *pp;
}

template <> std::string ptrstream_read<std::string>(const unsigned char*& p);

template <typename T> T* ptrstream_read_ptr(const unsigned char*& p) {
	T* pp = (T*)p;
	p += sizeof(T);
	return pp;
}

template <typename T> void ptrstream_read(const unsigned char*& p, T* out) {
	memcpy(out, p, sizeof(T));
	p += sizeof(T);
}

void SYNC_START();
void SYNC_END();



class AgmlException_Fatal : public std::runtime_error {public: AgmlException_Fatal(const std::string& what) : std::runtime_error(what) {}};
class AgmlException_Error : public std::runtime_error {public: AgmlException_Error(const std::string& what) : std::runtime_error(what) {}};
class AgmlException_Warning : public std::runtime_error {public: AgmlException_Warning(const std::string& what) : std::runtime_error(what) {}};

#endif /* UTILS_H_ */
