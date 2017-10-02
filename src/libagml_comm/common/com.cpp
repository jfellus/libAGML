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


#include "com.h"
#include "../tcp/Socket.h"
#include "Host.h"
#include "../util/array.h"
#include "../agml/node.h"
#include "../simulation/Thread.h"
#include "../server/Server.h"
#include "Commands.h"
#include "../topology/Topology.h"


///////////////////
// GLOBAL FIELDS //
///////////////////


std::string SERVER_IP;

array<Host*> hosts;

Host* root_host;
array<Host*> slaves;
array<Host*> masters;

//array<Host*> data_hosts;

array<Thread*> threads;
long NB_NODES = 0;




//////////
// INIT //
//////////

void com_init() {
	SERVER_IP = get_my_first_ip();
	DBG("My IP is " << SERVER_IP);
}

void com_exit() {
	server_stop();
}

bool AGML_IS_ROOT() {
	return masters.empty();
}

bool AGML_IS_ISOLATED() {
	return masters.empty() && slaves.empty();
}

bool com_is_my_ip(const std::string& ip) {
	if(str_has(ip, ":")) {
		std::string port = str_after(ip, ":");
		std::string realip = resolve_ip(str_before(ip, ":"));
		if(realip=="127.0.0.1") return TOINT(port)==SERVER_PORT;
		return SERVER_IP==realip && TOINT(port)==SERVER_PORT;
	}
	if(ip=="127.0.0.1") return true;
	return resolve_ip(ip)==SERVER_IP;
}

//////////////////////////
// ADDRESSES RESOLUTION //
//////////////////////////


/** @return the Node object associated with id <i>node_id</i> */
Node* com_decode_local_node(long node_id) {
	int node_group = (int)(node_id >> 32);
	int id = (int)node_id;
	if(id < 0) throw std::runtime_error(TOSTRING("Node id is negative ! (" << id << ")"));
	if(node_group < 0) throw std::runtime_error(TOSTRING("Node group is negative ! (" << node_group << ")"));

	NodeGroup* g = Topology::cur->get_group(node_group);
	if(!g) throw std::runtime_error(TOSTRING("No such group : " << node_group));
	if((uint)id >= g->nb_local_nodes) throw std::runtime_error(TOSTRING("NodeGroup " << g->name << " nodes index overflow : " << id << ">=" << g->nb_local_nodes));
	return g->get_local_node(id);
}

long com_encode_local_node(Node* n) {
	return (((long)n->node_group->id) << 32) | n->id;
}



/////////////////////
// SYSTEM COMMANDS //
/////////////////////


struct _AGML_Command { const char* name; void(*fn)(Host*, const char* data, size_t size); const char* ret_type; };
struct _AGML_Command AGML_COMMANDS[] = {
		{"add_to_network", agml_command_add_to_network, NULL},
		{"dump", agml_command_dump, NULL},
		{"echo", agml_command_echo, NULL},
		{"enter", agml_command_enter, NULL},
		{"exit", agml_command_exit, NULL},
		{"leave", agml_command_leave, NULL},
		{"localdump", agml_command_localdump, NULL},
		{"notify_subscription", agml_command_notify_subscription, NULL},
		{"root_welcome", agml_command_root_welcome, NULL},
		{"set_topology", agml_command_set_topology, NULL},
		{"shell", agml_command_shell, NULL},
		{"subscribe", agml_command_subscribe, NULL},
		{"model", agml_command_model, NULL},
		{"start_infos", agml_command_start_infos, NULL},
		{"do_start_infos", agml_command_do_start_infos, NULL},
		{"data_host", agml_command_data_host, NULL},
		{"update_infos", agml_command_update_infos, NULL},
		{"infos", agml_command_infos, NULL},
		{"infos_reply", agml_command_infos_reply, NULL},
		{"node_request", agml_command_node_request, "string"},
		{NULL,NULL,NULL}
};

int AGML_NB_COMMANDS() {
	static int nb_commands=0;
	if(nb_commands==0) for(uint i=0; AGML_COMMANDS[i].name!=NULL; i++) nb_commands++;
	return nb_commands;
}

int AGML_COMMAND_ID(const std::string& cmd_name) {
	for(uint i=0; AGML_COMMANDS[i].name!=NULL; i++) if(cmd_name==AGML_COMMANDS[i].name) return i;
	return -1;
}

std::string AGML_COMMAND_NAME(int command_id) {
	if(command_id < 0 || command_id >= AGML_NB_COMMANDS()) return "";
	return AGML_COMMANDS[command_id].name;
}

void AGML_COMMAND_EXEC(Host* host, const std::string& cmd_name, const char* data, size_t size) {
	for(uint i=0; AGML_COMMANDS[i].name!=NULL; i++) {
		if(AGML_COMMANDS[i].name==cmd_name) {
			AGML_COMMANDS[i].fn(host, data, size);
			return;
		}
	}
}

void AGML_COMMAND_EXEC(Host* host, int command_id, const char* data, size_t size) {
	if(command_id < 0 || command_id >= AGML_NB_COMMANDS()) {
		ERROR("ERROR : Received unknown command (#" << command_id << ")");
		return;
	}
	AGML_COMMANDS[command_id].fn(host, data, size);
}

bool AGML_COMMAND_HAS_RESPONSE(const std::string& cmd_name) {
	int i = AGML_COMMAND_ID(cmd_name);
	if(i<0) return false;
	return AGML_COMMANDS[i].ret_type!=NULL;
}


/** Send an AGML system command to the given host */
void com_send_command(int host_id, int command, const unsigned char* data, size_t size) {
	hosts[host_id]->send_command(command, data, size);
}

/** Send an AGML system command to the root host */
void com_send_command_root(int command, const unsigned char* data, size_t size) {
	root_host->send_command(command, data, size);
}

/** Send an AGML system command to the master hosts */
void com_send_command_masters(int command, const unsigned char* data, size_t size) {
	for(uint i=0; i<masters.size(); i++) masters[i]->send_command(command, data, size);
}

/** Send an AGML system command to the slave hosts */
void com_send_command_slaves(int command, const unsigned char* data, size_t size) {
	for(uint i=0; i<slaves.size(); i++) slaves[i]->send_command(command, data, size);
}




////////////////////////
// NETWORK MANAGEMENT //
////////////////////////

void com_model(const std::string& model) {
	agml_command_model(NULL, model.c_str(), model.length());
}

Host* com_enter_network(const std::string& bootstrap_ip) {
	Host* h = new Host(new Socket(bootstrap_ip.c_str(), true), true);
	masters.add(h);
	h->subscribe();
	return h;
}

Host* com_add_to_network(const std::string& slave_ip) {
	Host* h = new Host(new Socket(slave_ip.c_str(), true), true);
	slaves.add(h);

	DBG("Invite " << h->server_ip << " to join the network");
	h->send_sys_command("add_to_network");
	return h;
}


void com_root_on_subscribe(Host* subscriber) {
	subscriber->send_sys_command("root_welcome", TOSTRING(SERVER_PORT));
}





////////////////////////
// CONNECTION THREADS //
////////////////////////


void com_connection_thread(Host* h) {
	hosts.add(h);
	try {
		while(h->is_connected()) {
			Message* m = new Message();
			try {
			m->read(h);
			} catch (SocketClosedException& e) {break;}
			  catch(std::exception& e) {
				ERROR("ERROR receiving TCP data : " << e.what());
				exit(1);
			}
			h->on_receive(m);
		}
	} catch (SocketClosedException& e) {}
	catch(std::exception& e) {ERROR("ERROR : " << e.what());}
	catch(...) {ERROR("FATAL UNKNOWN ERROR");}

	if(h->data_host) ERROR("Data connection lost to " << h->data_host->host_name << " (ip=" << h->data_host->server_ip << ")");
	else DBG("Client " << h->server_ip << " left");
	hosts.remove(h);
	masters.remove(h);
	slaves.remove(h);
	if(h==root_host) root_host = 0;
	//DBG(com_dump_hosts());

	delete h; h = 0;
}


///////////
// DEBUG //
///////////

std::string com_dump_hosts() {
	std::ostringstream oss;
	for(uint i=0; i<hosts.size(); i++) {
		oss << hosts[i]->dump();
		oss << "\n";
	}
	return oss.str();
}


static std::string _copy_infos_path;
void* _copy_infos_thread(void*) {
	shell(TOSTRING("mkdir -p " << _copy_infos_path));
	for(;;) {
		shell(TOSTRING("cp -f /run/shm/agml* " << _copy_infos_path << " 2>/dev/null"));
		sleep(1);
	}
	return 0;
}

void start_copying_infos(const std::string& dst_path) {
	static bool started = false;
	static pthread_t th;
	if(!started) {
		_copy_infos_path = dst_path;
		pthread_create(&th, NULL, _copy_infos_thread, NULL);
		started = true;
	}
}

