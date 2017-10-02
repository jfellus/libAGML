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

#ifndef NODEGROUP_H_
#define NODEGROUP_H_

#include <map>
#include "../util/utils.h"
#include "../util/array.h"
#include "../agml/node.h"
#include "../common/Host.h"
#include "DataHost.h"
#include "Info.h"


class NodeGroupHost;
class NodeInfo;
class Link;

class NodeGroup {
public:
	int id;
	std::string name;
	std::string nodeclass;

	array<Link*> ins;
	array<Link*> outs;
	int nb_out_nodes;
	int nb_in_nodes;

	array<NodeGroupHost*> hosts;
	array<NodeGroupHost*> local_hosts;

	long nb_nodes;
	long nb_local_nodes;

	std::map<std::string, std::string> properties;

public:
	NodeGroup(const std::string& name, const std::string& nodeclass);

	///////////////
	// ACCESSORS //
	///////////////


	void assign(const std::string& host_name, long nb_nodes, int thread = -1);
	void assign_same_as(NodeGroup* g, int limit_nb_nodes_per_host = -1);

	void add_in(Link* l);
	void add_out(Link* l);
	void remove_in(Link* l);
	void remove_out(Link* l);
	Link* get_in(int i);
	Link* get_out(int i);

	bool has_local_in();
	bool has_local_out();

	NodeGroupHost* get_first_host(const std::string& host_name);
	Node* get_local_node(size_t id);


	template <typename T> void set_property(const std::string& key, T& val) { properties[key] = TOSTRING(val); }
	std::string get_property(const std::string& key) {
		if(!properties.count(key)) return "";
		return properties[key];
	}
	int get_property_int(const std::string& key, int default_val) {	return has_property(key) ? TOINT(get_property(key)) : default_val; }
	float get_property_float(const std::string& key, float default_val) { return has_property(key) ? TOFLOAT(get_property(key)) : default_val; }
	bool has_property(const std::string& key) { return properties.count(key)!=0; }

	///////////////////
	// COMMUNICATION //
	///////////////////


	bool send_out(Node* src, uint iNeighbor, Message& m);
	void send(Node* src, uint dst, Message& m);


	///////////
	// DEBUG //
	///////////

	std::string dump();
	std::string dump_outs();



private:
	friend class Topology;

	void add_host(NodeGroupHost* h);
	void connect_hosts_as_needed();
};




class NodeGroupHost {
public:
	NodeGroup* g;
	DataHost* host;

	array<Node*> nodes;
	array<NodeInfo*> remote_infos;
	size_t nb_nodes;

	NodeGroupHostInfo* infos;

	bool bError;
	int bIsLocal;


	/** Local */
	NodeGroupHost(NodeGroup* g) : g(g) {
		host = 0;
		nb_nodes = 0;
		infos = 0;
		bError = false;
		bIsLocal = true;
	}

	/** Remote */
	NodeGroupHost(NodeGroup* g, const std::string& host_name, long nb_nodes, int thread);
	NodeGroupHost(NodeGroup* g, DataHost* h, long nb_nodes, int thread);


	~NodeGroupHost() {}

	void init(NodeGroup* g, DataHost* h, long nb_nodes, int thread);
	void init_infos();

	inline bool is_local() {
		if(bIsLocal==-1) bIsLocal = host->is_local();
		return (bool) bIsLocal;
	}

	inline bool is_connected() {return host!=0 && host->is_connected();}
	inline bool is_error() {return bError;}

	std::string get_state();

	////////////
	// REMOTE //
	////////////

	void connect();

	/////////////////////////
	// LOCAL INSTANTIATION //
	/////////////////////////

	void add(Node* node);

	void instantiate_nodes(long nb_nodes, int thread = -1);
	void instantiate_nodes_same_as(NodeGroupHost* h, long limit = -1);

	inline bool instantiate_node(int thread) {	return instantiate_node(threads[thread]); }
	bool instantiate_node(Thread* t);


	////////////
	// EVENTS //
	////////////

	void on_process();
	void on_send(size_t size);
	void on_receive(size_t size);
};




class Link {
public:
	NodeGroup* src;
	NodeGroup* dst;
	bool bAllowSelf, bOnlyLocal;

	Link(NodeGroup* src, NodeGroup* dst, bool bAllowSelf = false, bool bOnlyLocal = false) : src(src),dst(dst),bAllowSelf(bAllowSelf),bOnlyLocal(bOnlyLocal) {

	}

	~Link() {}


	inline bool is_self() {return src==dst;}

	inline size_t get_nb_out() {
		if(bOnlyLocal) return get_nb_local_out();
		if(dst->nb_nodes==0) return 0;
		if(!bAllowSelf && is_self()) return dst->nb_nodes-1;
		return dst->nb_nodes;
	}

	inline size_t get_nb_in() {
		if(bOnlyLocal) return get_nb_local_in();
		if(src->nb_nodes==0) return 0;
		if(!bAllowSelf && is_self()) return src->nb_nodes-1;
		return src->nb_nodes;
	}

	inline size_t get_nb_local_out() {
		if(dst->nb_local_nodes==0) return 0;
		if(!bAllowSelf && is_self()) return dst->nb_local_nodes-1;
		return dst->nb_local_nodes;
	}

	inline size_t get_nb_local_in() {
		if(src->nb_local_nodes==0) return 0;
		if(!bAllowSelf && is_self()) return src->nb_local_nodes-1;
		return src->nb_local_nodes;
	}

	inline size_t get_out_node_id(size_t src, int iNeighbor) {
		if(bAllowSelf || !is_self()) return iNeighbor;
		if((size_t)iNeighbor >= src) return iNeighbor+1;
		return iNeighbor;
	}

	void on_dst_add_host(NodeGroupHost* h) {
		if(!bOnlyLocal || h->is_local())
			src->nb_out_nodes += (!bAllowSelf && is_self() && src->nb_nodes==0) ? h->nb_nodes-1 : h->nb_nodes;
	}

	void on_src_add_host(NodeGroupHost* h) {
		if(!bOnlyLocal || h->is_local())
			dst->nb_in_nodes += (!bAllowSelf && is_self() && dst->nb_nodes==0) ? h->nb_nodes-1 : h->nb_nodes;
	}
};


#endif /* NODEGROUP_H_ */
