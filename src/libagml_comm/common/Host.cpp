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

#include "Host.h"
#include "com.h"
#include "../simulation/Thread.h"
#include "../topology/Topology.h"
#include "../topology/DataHost.h"


Host::Host() {
	data_host = NULL;
	server_ip = "";
	thread = 0;
	bIsCommandsChannel = false;
	socket = 0;
	id = 0;
	next_node_id = 0;
}

Host::Host(Socket* s, bool _bIsCommandsChannel) {
	LOCK();
	data_host = NULL;
	server_ip = TOSTRING(s->ip << ":" << s->port);
	bIsCommandsChannel = _bIsCommandsChannel;
	id = 0;
	next_node_id = 0;
	socket = s;

	if(s->isClient()) {
		create_connection_thread();
	}
	//		DBG("New host joined the network : " << get_ip());
	UNLOCK();
}

Host::~Host() {
	LOCK();
	if(data_host) delete data_host;
	data_host = NULL;
	//		DBG("Host leaved the network : " << get_ip());
	if(socket) delete socket;
	socket = 0;
	UNLOCK();
}








std::string Host::dump() {
	std::string type = "";
	if(this->bIsCommandsChannel) type += "cmd"; else type += "data";

	if(this->is_local()) type += ",local";
	if(this == root_host) type += ",root";
	else if(this->is_slave()) type += ",slave";
	else if(this->is_master()) type += ",master";

	return TOSTRING(id << " | " << server_ip << " (" << type << ") req from " << get_ip());
}


///////////////
// LIFECYCLE //
///////////////

long Host::create_node_id() {
	return (((long)id) << 48) | next_node_id++;
}


extern void com_connection_thread(Host* h);
static void* _connection_thread_client(void* p) { com_connection_thread((Host*)p); return 0;}
void Host::create_connection_thread() {
	pthread_create(&thread, NULL, _connection_thread_client, this);
}


void Host::subscribe() {
	send_sys_command("subscribe", TOSTRING(SERVER_PORT));
}



////////////////////
// COMMUNICATIONS //
////////////////////

void Host::send(long src, long dst, int channel, const unsigned char* data, size_t size) {
	Message m(src, dst, channel, data, size);
	send(&m);
}

void Host::send_command(int command, const unsigned char* data, size_t size) {
	Message m((long)-1, (long)-1, command, data, size);
	send(&m);
}

void Host::send(const std::string& rawmsg) {
	LOCK();
	socket->write(rawmsg);
	UNLOCK();
}

void Host::recv(Message* m) {
	m->read(this);
}

void Host::send(Message* m) {
	LOCK();
	if(!socket) throw std::runtime_error(TOSTRING("Couldn't send to host " << server_ip));
	else m->write(socket);
	UNLOCK();
}


void Host::on_receive(Message* m) {
	if(m->is_sys_command()) {
		MessageElt& me = m->get_next();
		AGML_COMMAND_EXEC(this, m->channel, (const char*)me.data, me.size);
		delete m;
	}
	else com_decode_local_node(m->dst)->thread->push_message(m);
}


/////////////////////
// EVENTS HANDLING //
/////////////////////

void Host::on_subscribe() {

}
