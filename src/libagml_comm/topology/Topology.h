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

#ifndef TOPOLOGY_H_
#define TOPOLOGY_H_

#include "../common/com.h"
#include "../simulation/Thread.h"
#include "Info.h"
#include "NodeGroup.h"
#include <stdexcept>
#include <map>

class Host;
class Thread;
class NodeGroup;


class Topology {
public:
	array<NodeGroup*> groups;
	static Topology* cur;
public:
	Topology();
	virtual ~Topology();

	///////////////
	// ACCESSORS //
	///////////////

	size_t nb_nodes();
	size_t nb_group_hosts();



	void add(NodeGroup* g);
	NodeGroup* get_group(int id);
	NodeGroup* get_group(const std::string& groupname);

	void add_group(const std::string& name, const std::string& nodeclass) { add_group(new NodeGroup(name, nodeclass)); }
	void add_group(NodeGroup* g) { g->id = groups.size(); groups.add(g); }

	inline void set_group_property(const std::string& groupname, const std::string& key, const std::string& val) {
		NodeGroup* g = get_group(groupname);
		if(g) set_group_property(get_group(groupname), key, val);
		else ERROR("ERROR : No such NodeGroup : " << groupname);
	}
	inline void set_group_property(int node_group_id, const std::string& key, const std::string& val) { set_group_property(get_group(node_group_id), key, val); }
	inline void set_group_property(NodeGroup* g, const std::string& key, const std::string& val) { g->set_property(key, val); }

	inline void assign(const std::string& node_group, const std::string& host_name, long nb_nodes, int thread = -1)  {
		NodeGroup* g = get_group(node_group);
		if(g) assign(g, host_name, nb_nodes, thread);
		else ERROR("ERROR : No such NodeGroup : " << node_group);
	}
	inline void assign(int node_group_id, const std::string& host_name, long nb_nodes, int thread = -1) { assign(get_group(node_group_id), host_name, nb_nodes); }
	inline void assign(NodeGroup* g, const std::string& host_name, long nb_nodes, int thread = -1) { g->assign(host_name, nb_nodes, thread); }

	inline void assign_same_as(const std::string& node_group, const std::string& ref_group, int limit_nb_nodes_per_host = -1) {
		NodeGroup* g = get_group(node_group);
		NodeGroup* g2 = get_group(ref_group);
		if(!g) ERROR("ERROR : No such NodeGroup : " << node_group);
		else if(!g2) ERROR("ERROR : No such NodeGroup : " << ref_group);
		else g->assign_same_as(g2, limit_nb_nodes_per_host);
	}


	inline void connect(const std::string& src_node_group, const std::string& dst_node_group, bool bAllowSelf = false, bool bOnlyLocal = false) {
		NodeGroup* g1 = get_group(src_node_group);
		NodeGroup* g2 = get_group(dst_node_group);
		if(!g1) ERROR("ERROR : No such NodeGroup : " << src_node_group);
		else if(!g2) ERROR("ERROR : No such NodeGroup : " << dst_node_group);
		else connect(g1, g2, bAllowSelf, bOnlyLocal);
	}
	inline void connect(int src_group_id, int dst_group_id, bool bAllowSelf = false, bool bOnlyLocal = false) { connect(get_group(src_group_id), get_group(dst_group_id), bAllowSelf, bOnlyLocal); }
	void connect(NodeGroup* src, NodeGroup* dst, bool bAllowSelf = false, bool bOnlyLocal = false);


	////////
	// IO //
	////////

	void read(std::istream& s);
	void realize(const std::string& statement);

	size_t get_group_hosts_info(NodeGroupHostInfo* out);
	size_t get_infos(NodeInfo* out);

	void dump_groups();

	void dump_all_infos(Message* m);
	void dump_local_infos(Message* m);
	void update_infos(const unsigned char* msg_data);
};

#endif /* TOPOLOGY_H_ */
