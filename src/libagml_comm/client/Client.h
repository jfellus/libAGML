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

#ifndef CLIENT_H_
#define CLIENT_H_

#include "../common/com.h"
#include "../common/Message.h"
#include "../tcp/Socket.h"
#include <pthread.h>

class Client {
public:
	std::string server_ip;

	Socket* socket;
	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
public:
	Client(const std::string& server_ip = "localhost:10001");
	virtual ~Client();

	inline void LOCK() {pthread_mutex_lock(&mut);}
	inline void UNLOCK() {pthread_mutex_unlock(&mut);}




	//////////////
	// COMMANDS //
	//////////////

	inline void enter(const std::string& friend_ip) {send_sys_command("enter", friend_ip);}
	inline void leave() {send_sys_command("leave");}


	////////////////////
	// COMMUNICATIONS //
	////////////////////

	void send(Message* m);
	void send_sys_command(int command, const unsigned char* data, size_t size);
	inline void send_sys_command(int command, const std::string& msg) { send_sys_command(command, (const unsigned char*)msg.c_str(), msg.length()+1); }
	inline void send_sys_command(int command) { send_sys_command(command, 0, 0); }
	inline void send_sys_command(const std::string& cmd_name, const std::string& msg) { send_sys_command(AGML_COMMAND_ID(cmd_name), msg); }
	inline void send_sys_command(const std::string& cmd_name) { send_sys_command(AGML_COMMAND_ID((cmd_name))); }


};

#endif /* CLIENT_H_ */
