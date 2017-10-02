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

#include "../common/com.h"
#include "Topology.h"


Topology* Topology::cur = NULL;
Topology::Topology() {cur = this;}
Topology::~Topology() {}

size_t Topology::nb_nodes() {
	size_t n = 0;
	for(uint i=0; i<groups.size(); i++) n+=groups[i]->nb_nodes;
	return n;
}

size_t Topology::nb_group_hosts() {
	size_t n = 0;
	for(uint i=0; i<groups.size(); i++) {
		n += groups[i]->hosts.size();
	}
	return n;
}

void Topology::add(NodeGroup* g) {groups.add(g);}

NodeGroup* Topology::get_group(int id) {return (uint)id >= groups.size() ? NULL : groups[id];}

NodeGroup* Topology::get_group(const std::string& groupname) {
	for(uint i=0; i<groups.size(); i++) if(groups[i]->name==groupname) return groups[i];
	return NULL;
}

void Topology::connect(NodeGroup* src, NodeGroup* dst, bool bAllowSelf, bool bOnlyLocal) {
	Link* l = new Link(src,dst,bAllowSelf, bOnlyLocal);
	src->add_out(l);
	dst->add_in(l);
}


#include "TopologyReader.h"
void Topology::read(std::istream& s) {
	TopologyReader(s).read(this);
	for(uint i=0; i<groups.size(); i++) groups[i]->connect_hosts_as_needed();
	agml_threads_start();
}


void Topology::dump_groups() {
	for(uint i=0; i<groups.size(); i++) {
		for(uint j=0; j<groups[i]->hosts.size(); j++) {
			std::string state = groups[i]->hosts[j]->get_state();
			std::string group = str_align(groups[i]->name,10);
			std::string host = str_align(TOSTRING(groups[i]->hosts[j]->host->host_name << (groups[i]->hosts[j]->is_local() ? " (ME)" : "")), 20);
			std::string nodes = TOSTRING("[" << groups[i]->hosts[j]->nb_nodes << "]");
			DBG("  | " << group << " @ " << host << " " << nodes << " \t " << state);
		}
	}
}


void Topology::dump_all_infos(Message* m) {
	unsigned char* p = 0;
	for(int b=0; b<2; b++) {
		size_t nb_groups = groups.size();
		ptrstream_write(p, &nb_groups, 										 b==1);
		for(uint i=0; i<nb_groups; i++) {
			ptrstream_write(p, groups[i]->name, 							 b==1);
			ptrstream_write(p, groups[i]->nodeclass, 						 b==1);
			size_t nb_hosts = groups[i]->hosts.size();
			ptrstream_write(p, &nb_hosts, 									 b==1);
			for(uint j=0; j<nb_hosts; j++) {
				ptrstream_write(p, groups[i]->hosts[j]->host->host_name,	 b==1);
				ptrstream_write(p, &groups[i]->hosts[j]->nb_nodes,		 	 b==1);
				ptrstream_write(p, groups[i]->hosts[j]->infos, 				 b==1);
				for(uint k=0; k<groups[i]->hosts[j]->nb_nodes; k++) {
					if(groups[i]->hosts[j]->is_local())
						ptrstream_write(p, groups[i]->hosts[j]->nodes[k]->infos, b==1);
					else if(!groups[i]->hosts[j]->remote_infos.empty())
						ptrstream_write(p, groups[i]->hosts[j]->remote_infos[k], b==1);
					else ptrstream_write(p, (NodeInfo*)NULL, b==1);
				}
			}
		}
		if(!m->bDataAllocated) {
			size_t size = (size_t)(p-NULL);
			p = new unsigned char[size];
			m->bDataAllocated = true;
			m->add(p,size);
		}
	}
}

void Topology::dump_local_infos(Message* m) {
	unsigned char* p = 0;
	for(int b=0; b<2; b++) {
		size_t nb_groups = groups.size();
		ptrstream_write(p, &nb_groups, 												 b==1);
		for(uint i=0; i<nb_groups; i++) {
			ptrstream_write(p, groups[i]->name, 									 b==1);
			ptrstream_write(p, groups[i]->nodeclass, 								 b==1);
			size_t nb_local_hosts = groups[i]->local_hosts.size();
			ptrstream_write(p, &nb_local_hosts, 									 b==1);
			for(uint j=0; j<nb_local_hosts; j++) {
				ptrstream_write(p, groups[i]->local_hosts[j]->host->host_name,		 b==1);
				ptrstream_write(p, &groups[i]->local_hosts[j]->nb_nodes,		 	 b==1);
				ptrstream_write(p, groups[i]->local_hosts[j]->infos, 				 b==1);
				for(uint k=0; k<groups[i]->local_hosts[j]->nb_nodes; k++) {
					ptrstream_write(p, groups[i]->local_hosts[j]->nodes[k]->infos,	 b==1);
				}
			}
		}
		if(!m->bDataAllocated) {
			size_t size = (size_t)(p-NULL);
			p = new unsigned char[size];
			m->bDataAllocated = true;
			m->add(p, size);
		}
	}
}

void Topology::update_infos(const unsigned char* p) {
	size_t nb_groups = ptrstream_read<size_t>(p);
	for(size_t i=0; i<nb_groups; i++) {
		std::string group_name = ptrstream_read<std::string>(p);
		NodeGroup* g = get_group(group_name);
		if(!g) ERROR("No such group " << group_name);
		std::string cls = ptrstream_read<std::string>(p);
		size_t nb_hosts = ptrstream_read<size_t>(p);
		for(size_t j=0; j<nb_hosts; j++) {
			std::string host_name = ptrstream_read<std::string>(p);
			NodeGroupHost* h = g->get_first_host(host_name);
			if(!h) ERROR("No such group host " << group_name << "@" << host_name);
			size_t nb_nodes = ptrstream_read<size_t>(p);
			if(h->infos==NULL) h->infos = new NodeGroupHostInfo();
			ptrstream_read(p, h->infos);
			for(size_t k=0; k<nb_nodes; k++) {
				if(h->is_local()) {
					if(h->nodes[k]->infos==NULL) h->nodes[k]->infos = new NodeInfo();
					ptrstream_read(p, h->nodes[k]->infos);
				} else {
					if(h->remote_infos.empty()) for(size_t l = 0; l<h->nb_nodes; l++) h->remote_infos.add(new NodeInfo());
					ptrstream_read(p, h->remote_infos[k]);
				}
			}
		}
	}
}
