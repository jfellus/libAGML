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




#include "Message.h"
#include "Host.h"
#include <stdexcept>


Message::Message() {
	from = NULL;
	src = dst = 0;
	channel = 0;
	bDataAllocated = false;
	total_size = 0;
}

Message::Message(int channel) {
	this->channel = channel;
	from = NULL;
	src = dst = 0;
	bDataAllocated = false;
	total_size = 0;
}

Message::Message(long src, long dst, int channel, const unsigned char* data, size_t size) {
	from = NULL;
	this->src = src;
	this->dst = dst;
	this->channel = channel;
	total_size = 0;
	add((unsigned char*)data, size);
	i = elts.begin();
	bDataAllocated = false;
}

Message::Message(Message& m) {
	from = NULL;
	src = m.src;
	dst = m.dst;
	channel = m.channel;
	elts = m.elts;
	total_size = m.total_size;
	bDataAllocated = m.bDataAllocated;
	m.bDataAllocated = false;
}

Message::~Message() {
	if(bDataAllocated) {
		for(std::list<MessageElt>::iterator i=elts.begin(); i!=elts.end(); i++) {
			if((*i).bOwn && (*i).data) {delete[] (*i).data; (*i).data = NULL;}
		}
	}
}

bool Message::is_sys_command() {
	return src==((long)-1);
}

void Message::read(Socket* s) {
	//	char magic[5];
	//	h->socket->read_exactly(magic, 5);
	//	if(strcmp(magic, "AGML")) {
	//		DBG("ERROR:Wrong magic number : " << magic << " messages are in a mess !");
	//		exit(1);
	//		throw std::runtime_error(TOSTRING("Wrong magic number : " << magic << " messages are in a mess !"));
	//	}
	s->read(&src);
	s->read(&dst);
	s->read(&channel);
	ushort nb_data;
	s->read(&nb_data);
	for(ushort i = 0; i<nb_data; i++) {
		size_t size = 0;
		unsigned char* data = 0;
		s->read(&size);
		if(size>0) {
			data = new unsigned char[size];
			bDataAllocated = true;
			s->read_exactly((unsigned char*)data, size);
		} else size=0;
		add(data, size);
	}
	this->i = elts.begin();
}

void Message::read(Host* h) {
	from = h;
	read(h->socket);
}

void Message::write(Socket* s) {
	from = NULL;
//	s->write("AGML");
	s->write(src);
	s->write(dst);
	s->write(channel);

	s->write((ushort)elts.size());
	for(std::list<MessageElt>::iterator i = elts.begin();i!=elts.end();i++) {
		if((*i).size==0 || (*i).data==0) {
			s->write((size_t)0);
		} else {
			s->write((size_t)(*i).size);
			s->write((unsigned char*)(*i).data, (size_t)(*i).size);
		}
	}
}

std::string Message::dump() {
	return TOSTRING("from : " << from->server_ip << " : " << src << "->" << dst << " ch=" << channel << ", size=" << total_size);
}


void Message::set_command(const std::string& cmd) {
	int id = AGML_COMMAND_ID(cmd);
	if(id==-1) ERROR("No such command : " << cmd);
	else set_command(id);
}

Message* Message::copy() {
	Message* m = new Message();
	m->bDataAllocated = true;
	m->from = from;
	m->channel = channel;
	m->dst = dst;
	m->src = src;
	for(std::list<MessageElt>::iterator i = elts.begin(); i!=elts.end(); i++) {
		size_t s = (*i).size;
		unsigned char* buf = new unsigned char[s];
		memcpy(buf, (*i).data, s);
		m->add(buf, s);
	}
	return m;
}


