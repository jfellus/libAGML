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

#include "Commands.h"
#include "com.h"
#include "Host.h"
#include "../tcp/Socket.h"
#include "../topology/Topology.h"
#include "../topology/TopologyReader.h"
#include "../topology/Info.h"
#include "../topology/DataHost.h"

extern array<DataHost*> data_hosts;



////////////////////////////
// USER TERMINAL COMMANDS //
////////////////////////////

void agml_command_exit(Host* h, const char* s, size_t n) {
	DBG("BYE!");
	com_exit();
}

void agml_command_echo(Host* h, const char* s, size_t n) {
	DBG("Message : " << s);
}

void agml_command_shell(Host* h, const char* shell_cmd, size_t n) {
	shell(shell_cmd);
}

void agml_command_enter(Host* h, const char* peer_ip, size_t n) {
	try {
		com_enter_network(peer_ip);
	} catch(std::string& s) {ERROR("ERROR : " << s);}
}

void agml_command_leave(Host* h, const char* params, size_t n) {
}

void agml_command_dump(Host* h, const char* params, size_t n) {
//	DBG(TOSTRING("Hosts : \n" << com_dump_hosts()));
	h->send(com_dump_hosts());
}

void agml_command_localdump(Host* h, const char* params, size_t n) {
	DBG(TOSTRING("Hosts : \n" << com_dump_hosts()));
}


/////////////////////////////////
// NETWORK MANAGEMENT COMMANDS //
/////////////////////////////////

void agml_command_add_to_network(Host* host, const char* params, size_t n) {
	masters.add(host);
	slaves.remove(host);

	DBG("Invitation from " << host->server_ip);
}

void agml_command_subscribe(Host* subscriber, const char* params, size_t n) {
	subscriber->server_ip = TOSTRING(subscriber->socket->ip << ":" << params);

	DBG("New subscriber : " << subscriber->server_ip << " (requested from " << subscriber->get_ip() << ")");
	slaves.add(subscriber);
	subscriber->on_subscribe();

	if(AGML_IS_ROOT()) com_root_on_subscribe(subscriber);
	else com_send_command_masters("notify_subscription", subscriber->server_ip);

	DBG(com_dump_hosts());
}

void agml_command_notify_subscription(Host* from, const char* subscriber_server_ip, size_t n) {
	if(AGML_IS_ROOT()) {
		try {
			DBG("Create connection to subscriber @ " << subscriber_server_ip);
			Host* h = new Host(new Socket(subscriber_server_ip));
			com_root_on_subscribe(h);
			DBG(com_dump_hosts());
		} catch(std::string& s) {ERROR("ERROR : " << s);}
		catch(std::exception& e) {ERROR("ERROR : " << e.what());}
	}
	else com_send_command_root("notify_subscription", subscriber_server_ip);
}

void agml_command_root_welcome(Host* root, const char* params, size_t n) {
	root_host = root;
	slaves.remove(root_host);
	root->server_ip = TOSTRING(root->socket->ip << ":" << params);

	DBG("Root host spoke to me @ " << root->server_ip);
	DBG(com_dump_hosts());
}


//////////////////
// DATA STREAMS //
//////////////////


void agml_command_model(Host* h, const char* topology, size_t n) {
	DBG("\n__ MODEL REQUESTED _____________");
	// If isolated -> connect to all hosts specified in the topology
	if(AGML_IS_ISOLATED()) {
		DBG("Create connections to spread the model");
		std::istringstream f(topology);
		TopologyReader tr(f);
		tr.read_hosts();
		for(uint i=0; i<data_hosts.size(); i++) {
			DBG(" - " << data_hosts[i]->host_name << " (ip=" << data_hosts[i]->server_ip << ")"
					<< (data_hosts[i]->is_local() ? " <- ME" : ""));
			if(data_hosts[i]->is_local()) continue;
			try {
				Host* h = com_add_to_network(data_hosts[i]->server_ip);
				h->send_sys_command("root_welcome", TOSTRING(SERVER_PORT));
			} catch(std::exception& e) { ERROR("ERROR : " << e.what()); }
		}
	}

	// If root, starts flooding the network with our topology
	if(AGML_IS_ROOT()) agml_command_set_topology(NULL, topology, n);

	// If not root, first propagate the topology to the root
	else com_send_command_masters("model", topology);
}

void agml_command_set_topology(Host* h, const char* topology, size_t n) {
	DBG("Update Topology ... ");
	com_send_command_slaves("set_topology", topology);
	if(!Topology::cur) Topology::cur = new Topology();
	//DBG("\n\n________________________________\n" << topology << "\n___________________________\n\n");
	std::istringstream s(topology);
	Topology::cur->read(s);
	DBG("\n\x1b[34mTopology successfully updated");	Topology::cur->dump_groups();	DBG("\x1b[0m\n");
}

void agml_command_data_host(Host* h, const char* host_name, size_t n) {
	DBG("Connection from " << host_name << " (ip=" << h->server_ip << ")");
	DataHost* dh = agml_get_datahost(host_name);
	if(!dh) dh = new DataHost(host_name, h->server_ip);
	dh->host = h;
	h->data_host = dh;
}


///////////
// INFOS //
///////////


void agml_command_start_infos(Host* h, const char* params, size_t n) {
	if(AGML_IS_ROOT()) com_send_command_slaves("do_start_infos", params);
	else com_send_command_masters("start_infos", params);
}

void agml_command_do_start_infos(Host* h, const char* params, size_t n) {
	agml_infos_start_update_thread(params);
}

void agml_command_infos(Host* h, const char* params, size_t n) {
	//DBG("INFOS");
	if(AGML_IS_ROOT()) {
		if(Topology::cur==NULL) {
			ERROR("Infos requested but no Model has been set");
			h->send_command("infos_reply",0,0); return;
		}
		Message m; m.set_command("infos_reply");
		Topology::cur->dump_all_infos(&m);
		h->send(&m);
	} else if(root_host){
		root_host->send_sys_command("infos", params);
		Message m;
		root_host->recv(&m);
		h->send(&m);
	}
}

void agml_command_update_infos(Host* h, const char* params, size_t n) {
	if(Topology::cur) Topology::cur->update_infos((const unsigned char*)params);
}

void agml_command_infos_reply(Host* h, const char* params, size_t n) {
	agml_process_infos_reply((const unsigned char*)params, n);
}

void agml_process_infos_reply(const unsigned char* data, size_t size) {
	if(size==0) { ERROR("No Model has been set or an error happened !"); return; }

	std::cout << "[\n";
	size_t nb_groups = ptrstream_read<size_t>(data);
	for(uint i=0; i<nb_groups; i++) {
		if(i!=0) std::cout << " ,\n";
		std::cout << "  {";
		std::cout << " \"name\" : \"" << ptrstream_read<std::string>(data) << "\" ,\n";
		std::cout << " \"cls\" : \"" << ptrstream_read<std::string>(data) << "\" ,\n";
		std::cout << "    \"hosts\" : [\n";
		size_t nb_hosts = ptrstream_read<size_t>(data);
		for(uint j=0; j<nb_hosts; j++) {
			if(j!=0) std::cout << " ,\n";
			std::cout << "        { \"name\" : \"" << ptrstream_read<std::string>(data) << "\" , ";
			size_t nb_nodes = ptrstream_read<size_t>(data);
			std::cout << "\"nb_nodes\" : " << nb_nodes << " ,\n";
			NodeGroupHostInfo* info = ptrstream_read_ptr<NodeGroupHostInfo>(data);
			std::cout << "          \"infos\" : ";
			info->dump_JSON();
			std::cout << ",\n";
			std::cout << "          \"nodes\" : [\n";
			for(uint k=0; k<nb_nodes; k++) {
				if(k!=0) std::cout << ",\n";
				std::cout << "              ";
				NodeInfo* info = ptrstream_read_ptr<NodeInfo>(data);
				info->dump_JSON();
			}
			std::cout << "\n               ]\n";
			std::cout << "        }\n";
		}
		std::cout << "   ]";
		std::cout << " } ";
	}
	std::cout << "\n]\n";
}


///////////////////
// NODE REQUESTS //
///////////////////

void agml_command_node_request(Host* h, const char* params, size_t n) {
	std::string s(params);
	std::string groupname = str_trim(str_before(s, "@"));
	std::string ss = str_trim(str_after(s, "@"));
	std::string host = str_trim(str_before(ss,"["));
	std::string sss = str_trim(str_after(ss,"["));
	std::string node_id = str_trim(str_before(sss,"]"));
	std::string what = str_trim(str_after(sss, " "));

	DBG("REQUESTED : " << groupname << "@" << host << "[" << node_id << "] = " << what);
	Message m;
	NodeGroup* g = Topology::cur->get_group(groupname);
	if(!g) throw std::runtime_error(TOSTRING("No such NodeGroup : " << groupname));
	NodeGroupHost* ngh = g->get_first_host(host);
	if(!ngh) throw std::runtime_error(TOSTRING("No such Host : " << groupname << "@" << host));
	if(ngh->is_local()) {
		Node* node = ngh->nodes[TOINT(node_id)];
		if(!node) throw std::runtime_error(TOSTRING("No such node : " << groupname << "@" << host << "[" << node_id << "]"));
		node->on_request(what, &m);
		if(m.isEmpty()) m.add("done");
	} else {
		m.add("ERROR : Host is not local !!");
	}
	h->send(&m);
}

