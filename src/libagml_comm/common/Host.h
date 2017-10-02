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

#ifndef AGML_HOST_H_
#define AGML_HOST_H_

#include "../tcp/Socket.h"
#include "com.h"
#include "../util/utils.h"
#include <pthread.h>


class DataHost;

class Host {
public:
	int id;
	Socket* socket;

	bool bIsCommandsChannel;
	std::string server_ip;

	pthread_t thread;
	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

	DataHost* data_host;
public:
	Host();
	Host(Socket* s, bool _bIsCommandsChannel = false);
	~Host();


	///////////////
	// ACCESSORS //
	///////////////

	inline bool is_connected() { return socket!=0 && socket->isConnected(); }
	inline bool is_local() 	{	return socket==0;	}
	inline bool is_slave() { return socket->isServer(); }
	inline bool is_master() { return socket->isClient(); }


	std::string get_ip() {
		if(!socket) return "-";
		return TOSTRING(socket->ip << ":" << socket->port);
	}

	std::string dump();


	///////////////
	// LIFECYCLE //
	///////////////

	long create_node_id();

	inline void LOCK() {pthread_mutex_lock(&mut);}
	inline void UNLOCK() {pthread_mutex_unlock(&mut);}


	void subscribe();


	/////////////////////
	// EVENTS HANDLING //
	/////////////////////

	void on_subscribe();

	////////////////////
	// COMMUNICATIONS //
	////////////////////

	void send(const std::string& raw_msg);
	void send(Message* m);
	void send(long src, long dst, int channel, const unsigned char* data, size_t size);
	void send_command(int cmd_id, const unsigned char* data, size_t size);
	inline void send_command(const std::string& cmd, const unsigned char* data, size_t size) {
		int id = AGML_COMMAND_ID(cmd);
		if(id==-1) {ERROR("ERROR : No such command : " << cmd); return;}
		send_command(AGML_COMMAND_ID(cmd), data, size);
	}
	inline void send_sys_command(int cmd_id, const std::string& params) {send_command(cmd_id, (const unsigned char*)params.c_str(), params.length()+1);}
	inline void send_sys_command(int cmd_id) {send_command(cmd_id, 0,0);}
	inline void send_sys_command(const std::string& cmd_name, const std::string& params) {
		int id = AGML_COMMAND_ID(cmd_name);
		if(id==-1) {ERROR("ERROR : No such command : " << cmd_name); return;}
		send_sys_command(AGML_COMMAND_ID(cmd_name), params);
	}

	inline void send_sys_command(const std::string& cmd_name) {
		int id = AGML_COMMAND_ID(cmd_name);
		if(id==-1) {ERROR("ERROR : No such command : " << cmd_name); return;}
		send_sys_command(id);
	}

	/** Synchronous reception (wait for message) */
	void recv(Message* m);

	/** Asynchronous reception */
	void on_receive(Message* m);

private:
	int next_node_id;

	void create_connection_thread();
};



#endif /* AGML_HOST_H_ */
