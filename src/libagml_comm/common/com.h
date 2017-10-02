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

#ifndef AGML_COM_H_
#define AGML_COM_H_

#include "../tcp/Socket.h"
#include "Message.h"
#include <vector>
#include "../util/array.h"

class Host;
class Thread;
class Node;


///////////////////
// Global fields //
///////////////////


extern std::string SERVER_IP;
extern unsigned short SERVER_PORT;

extern array<Host*> hosts;

extern Host* root_host;
extern array<Host*> slaves;
extern array<Host*> masters;

//extern array<Host*> data_hosts;

extern array<Thread*> threads;

extern long NB_NODES;


///////////////
// Lifecycle //
///////////////

void com_init();
void com_exit();


bool AGML_IS_ROOT();
bool AGML_IS_ISOLATED();


//////////////
// Commands //
//////////////

int AGML_COMMAND_ID(const std::string& cmd_name);
std::string AGML_COMMAND_NAME(int command_id);
void AGML_COMMAND_EXEC(Host* host, const std::string& cmd_name, const char* data, size_t size);
void AGML_COMMAND_EXEC(Host* host, int command_id, const char* data, size_t size);
bool AGML_COMMAND_HAS_RESPONSE(const std::string& cmd_name);


//////////////////////////
// Addresses resolution //
//////////////////////////

#define NODE_THREAD(node) (node%threads.size())
#define NODE_LOCALID(node) (node/threads.size())

/** @return the id associated with this Node object */
long com_encode_local_node(Node* n);

/** @return the Node object associated with id <i>node_id</i> */
Node* com_decode_local_node(long node_id);

/** @return true if the given ip correspond to this machine */
bool com_is_my_ip(const std::string& ip);


////////////////////////
// Messages exchanges //
////////////////////////


/**
 * Send a message from node <i>src</i> to node <i>dst</i>
 * @param src : source node
 * @param dst : destination node
 * @param channel : the channel to send to
 * @param data : data buffer to send
 * @param size : size of <i>data</i>
 */
void com_send(long src, long dst, int channel, const unsigned char* data, size_t size);

/** Send an AGML system command to the given host */
void com_send_command(int host_id, int cmd_id, const unsigned char* data, size_t size);
inline void com_send_command(int host_id, int cmd_id, const std::string& params = "") { com_send_command(host_id, cmd_id, (const unsigned char*)params.c_str(), params.length()+1); }
inline void com_send_command(int host_id, const std::string& cmd_name, const std::string& params = "") { com_send_command(host_id, AGML_COMMAND_ID(cmd_name), params); }

/** Send an AGML system command to the master hosts */
void com_send_command_masters(int command, const unsigned char* data, size_t size);
inline void com_send_command_masters(int cmd_id, const std::string& params = "") { com_send_command_masters(cmd_id, (const unsigned char*)params.c_str(), params.length()+1); }
inline void com_send_command_masters(const std::string& cmd_name, const std::string& params = "") { com_send_command_masters(AGML_COMMAND_ID(cmd_name), params); }

/** Send an AGML system command to the slave hosts */
void com_send_command_slaves(int command, const unsigned char* data, size_t size);
inline void com_send_command_slaves(int cmd_id, const std::string& params = "") { com_send_command_slaves(cmd_id, (const unsigned char*)params.c_str(), params.length()+1); }
inline void com_send_command_slaves(const std::string& cmd_name, const std::string& params = "") { com_send_command_slaves(AGML_COMMAND_ID(cmd_name), params); }

/** Send an AGML system command to the root host */
void com_send_command_root(int command, const unsigned char* data, size_t size);
inline void com_send_command_root(int cmd_id, const std::string& params = "") { com_send_command_root(cmd_id, (const unsigned char*)params.c_str(), params.length()+1); }
inline void com_send_command_root(const std::string& cmd_name, const std::string& params = "") { com_send_command_root(AGML_COMMAND_ID(cmd_name), params); }


////////////////////////
// NETWORK MANAGEMENT //
////////////////////////

void com_model(const std::string& topology);

Host* com_enter_network(const std::string& bootstrap_ip);
Host* com_add_to_network(const std::string& slave_ip);

void com_root_update_hosts();
void com_root_on_subscribe(Host* subscriber);



///////////
// DEBUG //
///////////

std::string com_dump_hosts();

void start_copying_infos(const std::string& dst_path);

#endif /* AGML_COM_H_ */
