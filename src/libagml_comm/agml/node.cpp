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

#include "../util/utils.h"
#include "../util/array.h"
#include "node.h"
#include "../simulation/Thread.h"
#include "../topology/Info.h"
#include "../topology/Topology.h"
#include <math.h>

#define SIZE 1000000
char DATA[SIZE];



Node::Node() {
	bInited = false;
	infos = NULL;
	id = -1;
	thread = 0;
	node_group = 0;
	host = NULL;
	bAttached = false;
	bFinished = false;
}

Node::~Node() {}

void Node::_init() {
	std::string f = TOSTRING("/agml_" << node_group->name << "@" << host->host->host_name << "_" << thread->id << "_node_" << id << ".log");
	infos = (NodeInfo*) shared_mem(f, sizeof(NodeInfo));
	if(!infos) throw std::runtime_error(TOSTRING("Couldn't init shared memory for NodeInfo : " << f));
	infos->init();

	try {init();}
	catch(AgmlException_Fatal& e) {	ERROR("FATAL ERROR in init() :  node " << dump() << " : " << e.what()); exit(1);}
	catch(AgmlException_Warning& e) {	ERROR("WARNING in init() :  node " << dump() << " : " << e.what()); }
	catch(std::exception& e) {	ERROR("ERROR in init() : node " << dump() << " : " << e.what());	}
}

void Node::_process() {
	long t = get_time_ms();
	if(t-infos->lasttime > 1000) {
		infos->ips = ((int)(infos->nbprocess_second)*10/((t-infos->lasttime)/1000))/10.0;
		infos->Ko_s = ((long)(infos->Ko_second*10)/((t-infos->lasttime)))/10.0;
		infos->Ko_r = ((long)(infos->Ko_r_second*10)/((t-infos->lasttime)))/10.0;
		infos->lasttime = t;
		infos->nbprocess_second = 0;
		infos->Ko_second = 0;
		infos->Ko_r_second = 0;
	}
	infos->nbprocess_second++;
	infos->nbProcess++;
	host->on_process();

	try {process();}
	catch(AgmlException_Fatal& e) {	ERROR("FATAL ERROR in process() :  node " << dump() << " : " << e.what()); exit(1);}
	catch(AgmlException_Warning& e) {	ERROR("WARNING in process() :  node " << dump() << " : " << e.what()); }
	catch(std::exception& e) {	ERROR("ERROR in process() : node " << dump() << " : " << e.what());	}
}

long Node::get_nb_outs() {
	return node_group->nb_out_nodes;
}



bool Node::send(int iNeighbor, Message& m) {
	if(!node_group->send_out(this, iNeighbor, m)) return false;
	infos->nbSend++;
	infos->Ko_second+=m.total_size;
	host->on_send(m.total_size);
	return true;
}

void Node::_receive(Message* m) {
	m->begin();
	if(!bInited) {bInited=true;_init();}
	infos->nbRecv++;
	infos->Ko_r_second+=m->total_size;
	host->on_receive(m->total_size);

	try {on_receive(m);}
	catch(AgmlException_Fatal& e) {	ERROR("FATAL ERROR in on_receive() :  node " << dump() << " : " << e.what()); exit(1);}
	catch(AgmlException_Warning& e) {	ERROR("WARNING in on_receive() :  node " << dump() << " : " << e.what()); }
	catch(std::exception& e) {	ERROR("ERROR in on_receive() : node " << dump() << " : " << e.what());	}

}


std::string Node::dump() {
	return TOSTRING(nodeclass << "_" << id << " @thread=" << thread->id);
}

std::string Node::get_property(const std::string& key) {
	return node_group->get_property(key);
}

float Node::get_property_float(const std::string& key, float default_val) {
	return node_group->get_property_float(key, default_val);
}

int Node::get_property_int(const std::string& key, int default_val) {
	return node_group->get_property_int(key, default_val);
}

bool Node::has_property(const std::string& key) {
	return node_group->has_property(key);
}

std::string Node::get_group_name() { return node_group->name; }
std::string Node::get_host_name() { return host->host->host_name; }
int Node::get_thread() { return thread->id; }


void Node::attach() {
	if(bAttached) return;
	thread->attach(this);
	bAttached = true;
	if(infos) infos->bAttached = true;
}

void Node::detach() {
	if(!bAttached) return;
	thread->detach(this);
	bAttached = false;
	if(infos) infos->bAttached = false;
}

void Node::finish() {
	if(bFinished) return;
	bFinished = true;
	thread->remove(this);
}

void Node::set_info_1(float val) { infos->var1 = val; }
void Node::set_info_2(float val) { infos->var2 = val; }
