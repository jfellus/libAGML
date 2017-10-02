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

#ifndef AGML_MESSAGE_H_
#define AGML_MESSAGE_H_

#include <string>
#include <stdlib.h>
#include <list>
#include <stdexcept>
#include "../util/utils.h"

class Socket;
class Host;

class MessageElt {
public:
	unsigned char* data;
	size_t size;
	bool bOwn;
	MessageElt(unsigned char* data, size_t size, bool bDisown = false) : data(data), size(size) {	bOwn = !bDisown; }
	~MessageElt() {}
};

class Message {
public:
	Host* from;
	long src,dst;
	int channel;

	std::list<MessageElt> elts;

	size_t total_size;
	bool bDataAllocated;

private:
	std::list<MessageElt>::iterator i;


public:
	Message();
	Message(int channel);
	Message(long src, long dst, int channel, const unsigned char* data, size_t size);
	Message(Message& m);
	~Message();

	bool is_sys_command();
	void set_command(const std::string& cmd);
	inline void set_command(int cmd_id) { src=dst=-1; channel = cmd_id; }

	Message* copy();

	inline void add(unsigned char* data, size_t size, bool bDisown = false) {
		elts.push_back(MessageElt(data, size, bDisown));
		total_size += size;
	}

	inline MessageElt& get_next(bool bDisown = false) {
		if(i==elts.end()) throw std::runtime_error("Message data overflow : data");
		(*i).bOwn = !bDisown;
		return *i++;
	}


	template <typename T> void add(const T& t) { add((unsigned char*)&t, sizeof(T)); }
	void add(const std::string& s) { add((unsigned char*)s.c_str(), s.length()+1, false); }
	inline void add(const float* t, size_t nb, bool bDisown = false) { add((unsigned char*)t, sizeof(float)*nb, bDisown); }

	void begin() {i = elts.begin();}
	template <typename T> T get() {return *((T*)(get_next().data));	}


	inline bool isEmpty() { return elts.empty(); }

	void read(Socket* s);
	void read(Host* h);

	void write(Socket* s);

	std::string dump();
};



#endif /* AGML_MESSAGE_H_ */
