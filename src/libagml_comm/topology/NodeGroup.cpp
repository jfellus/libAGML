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

#include "NodeGroup.h"
#include "../common/com.h"
#include "../simulation/Thread.h"
#include "Info.h"
#include "NodeLibrary.h"
#include <stdexcept>

/////////////////
// NODE GROUPS //
/////////////////

NodeGroup::NodeGroup(const std::string& name, const std::string& nodeclass) : name(name), nodeclass(nodeclass) {
	nb_out_nodes = 0;
	nb_in_nodes = 0;
	id = -1;
	nb_local_nodes = 0;
	nb_nodes = 0;
}



// TOPOLOGY IN / OUT

Link* NodeGroup::get_in(int i) {return ins[i];}
Link* NodeGroup::get_out(int i) {return outs[i];}

void NodeGroup::add_in(Link* l) {
	ins.add(l);
	nb_in_nodes += l->get_nb_in();
}

void NodeGroup::add_out(Link* l) {
	outs.add(l);
	l->dst->connect_hosts_as_needed();
	nb_out_nodes += l->get_nb_out();
}

void NodeGroup::remove_in(Link* in) {
	this->ins.remove(in);
	throw std::runtime_error("NOT IMPLEMENTED YET !");
}

void NodeGroup::remove_out(Link* out) {
	this->outs.remove(out);
	throw std::runtime_error("NOT IMPLEMENTED YET !");
}



// ASSIGN

/** @param thread : <0 evenly distribute, >0 this thread */
void NodeGroup::assign(const std::string& host_name, long nb_nodes, int thread) {
	add_host(new NodeGroupHost(this, host_name, nb_nodes, thread));
}

void NodeGroup::assign_same_as(NodeGroup* g, int limit_nb_nodes_per_host) {
	for(uint i=0; i<g->hosts.size(); i++) {
		if(g->hosts[i]->is_local()) {
			NodeGroupHost* h = new NodeGroupHost(this, g->hosts[i]->host, 0, -1);
			h->instantiate_nodes_same_as(g->hosts[i], limit_nb_nodes_per_host);
			add_host(h);
		}
		else {
			int nb = g->hosts[i]->nb_nodes;
			if(limit_nb_nodes_per_host!=-1) nb = MIN(limit_nb_nodes_per_host, nb);
			add_host(new NodeGroupHost(this, g->hosts[i]->host, nb, -1));
		}
	}
}



// LOCAL

bool NodeGroup::has_local_in() {
	for(uint i=0; i<ins.size(); i++) {
		if(!ins[i]->bAllowSelf && ins[i]->is_self() && ins[i]->src->nb_local_nodes > 1) return true;
		else if(ins[i]->src->nb_local_nodes > 0) return true;
	}
	return false;
}

bool NodeGroup::has_local_out() {
	for(uint i=0; i<outs.size(); i++) {
		if(!outs[i]->bAllowSelf && outs[i]->is_self() && outs[i]->dst->nb_local_nodes > 1) return true;
		if(outs[i]->dst->nb_local_nodes > 0) return true;
	}
	return false;
}

Node* NodeGroup::get_local_node(size_t id) {
	for(uint i=0; i<local_hosts.size(); i++) {
		if(id < local_hosts[i]->nb_nodes) return local_hosts[i]->nodes[id];
		id -= local_hosts[i]->nb_nodes;
	}
	return NULL;
}



// HOSTS

NodeGroupHost* NodeGroup::get_first_host(const std::string& host_name) {
	for(uint i=0; i<hosts.size(); i++) {
		if(hosts[i]->host->host_name == host_name) return hosts[i];
	}
	return NULL;
}


// COMMUNICATION

bool NodeGroup::send_out(Node* src, uint iNeighbor, Message& m) {
	uint _iNeighbor = iNeighbor;
	m.src = (((long)id << 32) | src->id);
	for(uint i=0; i<outs.size(); i++) {
		if(_iNeighbor < outs[i]->get_nb_out()) {
			try {
				outs[i]->dst->send(src, outs[i]->get_out_node_id(src->id, _iNeighbor), m);
				return true;
			} catch(std::exception& e) {return false;}
		}
		_iNeighbor -= outs[i]->get_nb_out();
	}
	throw std::runtime_error(TOSTRING("Neighbor overflow for group " << name << " out n°" << iNeighbor << " (outs are ["<< dump_outs() << "])"));
}

void NodeGroup::send(Node* src, uint dst, Message& m) {
	if(dst<0 || dst>=nb_nodes) throw std::runtime_error(TOSTRING("Node id overflow for group " << name << " node n°" << dst));
	if(dst<nb_local_nodes) {
		Node* n = get_local_node(dst);
		m.dst = (((long)id) << 32) | dst;
		if(src->thread == n->thread) n->_receive(&m);
		else n->thread->push_message(m.copy());
	} else {
		dst -= nb_local_nodes;
		for(uint i=0; i<hosts.size(); i++) {
			if(hosts[i]->is_local()) continue;
			if(dst < hosts[i]->nb_nodes) {
				if(!hosts[i]->host->is_connected()) throw std::runtime_error(TOSTRING("Couldn't send to " << hosts[i]->host->host_name << " : " << "DataHost not connected"));
				m.dst = (((long)id) << 32) | dst;
				hosts[i]->host->host->send(&m);
				return;
			}
			dst -= hosts[i]->nb_nodes;
		}
		throw std::runtime_error(TOSTRING("Remote node id overflow for group " << name << " node n°" << dst));
	}
}


// DEBUG

std::string NodeGroup::dump() {
	std::ostringstream oss;
	oss << "[";
	for(uint i=0; i<ins.size(); i++) {
		if(i!=0) oss << ", ";
		oss << ins[i]->src->name;
	}
	oss << "]" << name << "[";
	for(uint i=0; i<outs.size(); i++) {
		if(i!=0) oss << ", ";
		oss << outs[i]->dst->name;
	}
	oss << "]";
	return oss.str();
}

std::string NodeGroup::dump_outs() {
	std::ostringstream oss;
	oss << "[";
	for(uint i=0; i<outs.size(); i++) {
		if(i!=0) oss << ", ";
		oss << outs[i]->dst->name << ":" << outs[i]->get_nb_out();
	}
	oss << "]";
	return oss.str();
}



// P R I V A T E //


void NodeGroup::add_host(NodeGroupHost* h) {
	if(h->is_local()) local_hosts.add(h);
	hosts.add(h);
	for(uint i=0; i<ins.size(); i++) ins[i]->on_dst_add_host(h);
	for(uint i=0; i<outs.size(); i++) outs[i]->on_src_add_host(h);
	this->nb_nodes += h->nb_nodes;
}

void NodeGroup::connect_hosts_as_needed() {
	bool needed = false;
	for(uint i=0; i<ins.size(); i++) {
		if(ins[i]->bOnlyLocal) continue;
		if(ins[i]->get_nb_local_in()>0) {needed = true; break;}
	}
	if(!needed) return;

	for(uint i=0; i<hosts.size(); i++) {
		if(hosts[i]->is_connected() || hosts[i]->is_local()) continue;
		try {	hosts[i]->connect(); }
		catch(std::exception& e) { ERROR("ERROR: Couldn't connect to host " << hosts[i]->host->host_name); hosts[i]->host = 0;}
	}
}














/////////////////////////////



NodeGroupHost::NodeGroupHost(NodeGroup* g, const std::string& host_name, long nb_nodes, int thread) {
	this->g = 0;
	this->host = 0;
	this->infos = NULL;
	this->nb_nodes = 0;

	DataHost* h = agml_get_datahost(host_name);
	if(h==NULL) throw std::runtime_error(TOSTRING("No such host : " << host_name));
	init(g, h, nb_nodes, thread);
}

NodeGroupHost::NodeGroupHost(NodeGroup* g, DataHost* h, long nb_nodes, int thread) {
	init(g,h,nb_nodes, thread);
}

void NodeGroupHost::init(NodeGroup* g, DataHost* h, long nb_nodes, int thread) {
	this->g = g;
	this->host = h;
	this->nb_nodes = 0;
	this->infos = 0;
	bError = false;
	bIsLocal = -1;

	if(host->is_local()) {
		if(nb_nodes>0) {
			instantiate_nodes(nb_nodes, thread);
			if(this->nb_nodes==0) bError = true;
		}
		bIsLocal = true;
		init_infos();
	}
	else this->nb_nodes = nb_nodes;
}

void NodeGroupHost::init_infos() {
	std::string f = TOSTRING("/agml_" << g->name << "@" << host->host_name << ".log");
	infos = (NodeGroupHostInfo*) shared_mem(f, sizeof(NodeGroupHostInfo));
	infos->init();
}

std::string NodeGroupHost::get_state() {
	std::string state = "ERROR";
	if(bError) return state;
	if(is_local()) state = "local";
	else if(is_connected()) state = "connected";
	else if(!is_error()) state = "not connected";
	return state;
}



// REMOTE


void NodeGroupHost::connect() {
	if(host->is_local() || host->is_connected()) return;
	bError = true;
	DBG("Connect to " << g->name << "@" <<  host->host_name << " (ip=" << host->server_ip << ")");
	host->connect();
	bIsLocal = false;
	bError = false;
}



// Local


/**
 * @param thread : <0 = evenly distribute on all threads
 * (rem: the lightest thread gets more nodes if nb_nodes is not a multiple of the number of threads)
 */
void NodeGroupHost::instantiate_nodes(long nb_nodes, int thread) {
	if(thread < 0) {
		int th = threads_get_lightest();

		int nb_per_thread = nb_nodes / threads.size();
		int nb_remaining = nb_nodes - nb_per_thread*threads.size();

		if(nb_remaining > 0) instantiate_nodes(nb_remaining, th);
		if(nb_per_thread > 0) {
			for(uint i=0; i<threads.size(); i++) instantiate_nodes(nb_per_thread, (i+th)%threads.size());
		}
	} else {
		if(nb_nodes<0) nb_nodes = 1;

		DBG("Instantiate " << nb_nodes << " nodes for group " << g->name << " (on thread "<< thread << ")");

		threads[thread]->LOCK();
		for(uint i=0; i<nb_nodes; i++) instantiate_node(thread);
		threads[thread]->UNLOCK();
	}
}

void NodeGroupHost::instantiate_nodes_same_as(NodeGroupHost* h, long limit) {
	long nb = 0;
	DBG("Instantiate " << h->nb_nodes << " nodes for group " << g->name << " exactly as " << h->g->name);
	for(uint i=0; i<h->nb_nodes && (limit==-1 || nb<limit); i++) {
		h->nodes[i]->thread->LOCK();
		if(!instantiate_node(h->nodes[i]->thread)) return;
		h->nodes[i]->thread->UNLOCK();
		nb++;
	}
}

bool NodeGroupHost::instantiate_node(Thread* t) {
	Node *n = node_library_instanciate(g->nodeclass);
	if(!n) {
		ERROR("Couldn't instantiate node class " << g->nodeclass);
		return false;
	}
	n->nodeclass = g->nodeclass;
	add(n);
	n->thread = t;
	n->init();
	t->add(n);
	return true;
}

void NodeGroupHost::add(Node* node) {
	node->node_group = g;
	node->host = this;
	node->id = g->nb_local_nodes++;
	nodes.add(node);
	nb_nodes++;
}



// Events

void NodeGroupHost::on_process() {
	if(!infos) return;
	infos->update();
	infos->nbprocess_second++;
	infos->nbProcess++;
}

void NodeGroupHost::on_send(size_t size) {
	if(!infos) return;
	infos->nbSend++;
	infos->Ko_second+=size;
}

void NodeGroupHost::on_receive(size_t size) {
	if(!infos) return;
	infos->nbRecv++;
	infos->Ko_r_second+=size;
}



