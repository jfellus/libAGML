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

#include "utils.h"
#include <execinfo.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>



#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


DebugStream* DEBUG_STREAM = new DebugStreamStd();
DebugStream* ERROR_STREAM = new DebugStreamStd();


void PRINT_STACK_TRACE() {
	void *array[10];
	size_t size = backtrace(array, 10);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
}

std::string GET_STACK_TRACE() {
	void *array[10];
	size_t size = backtrace(array, 10);
	char** s = backtrace_symbols(array, size);
	std::string str = "";
	for(uint i=0; i<size; i++) {
		if(s[i]) str += s[i]; str += "\n";
	}
	return str;
}

std::string get_cur_thread_name() {
	char name[1000]; pthread_getname_np(pthread_self(), name, 1000);
	return name;
}


static struct timeval tstart, tend;
void TIC() {
	gettimeofday(&tstart, NULL);
}

void TAC() {
	gettimeofday(&tend, NULL);
	float ms = (tend.tv_sec - tstart.tv_sec)*1000.0 + (tend.tv_usec - tstart.tv_usec)/1000.0;
	float fps = 1000.0 / ms;
	DBG(ms << "ms " << fps << "fps");
}

long get_time_seconds() {
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec;
}

long get_time_ms() {
	struct timeval t;
	gettimeofday(&t, NULL);
	return (long)((t.tv_sec*1000.0) + (t.tv_usec/1000.0));
}

std::string str_date() {
	time_t _tm =time(NULL );
	struct tm * curtime = localtime ( &_tm );
	char* s = asctime(curtime);
	if(s[strlen(s)-1]=='\n') s[strlen(s)-1]=0;
	return s;
}


void* shared_mem(const std::string& path, size_t size) {
	int fd = shm_open(path.c_str(), O_RDWR | O_CREAT,S_IRUSR | S_IWUSR);
	if (fd == -1) return NULL;
	if (size!=(size_t)-1 && ftruncate(fd, size) == -1) return NULL;
	void* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(ptr==MAP_FAILED) return NULL;
	else return ptr;
}



template <> std::string ptrstream_read<std::string>(const unsigned char*& p) {
	size_t l = ptrstream_read<size_t>(p);
	char* s = new char[l+1]; s[l] = 0;
	std::string ss;
	memcpy(s,p,l);
	ss = s;
	p += l;
	delete [] s;
	return ss;
}

static pthread_mutex_t sync_mut = PTHREAD_MUTEX_INITIALIZER;
void SYNC_START() {
	pthread_mutex_lock(&sync_mut);
}

void SYNC_END() {
	pthread_mutex_unlock(&sync_mut);
}
