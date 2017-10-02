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

#ifndef NODE_NODE_H_
#define NODE_NODE_H_

#include "../common/Message.h"
#include "../util/utils.h"
#include <string>
#define INTERNAL

class Node;
class NodeGroup;
class NodeGroupHost;
class Thread;
class NodeInfo;
class Message;


#define AGML_NODE_CLASS(cls) 	extern "C" { Node* __agml_node_instanciator_##cls() { return new cls(); } }


#define AGML_FATAL_ERROR(x) throw AgmlException_Fatal(TOSTRING(x))
#define AGML_ERROR(x) throw AgmlException_Error(TOSTRING(x))
#define AGML_WARNING(x) throw AgmlException_Warning(TOSTRING(x))


#define record_var(x) _record_var(#x, x)
#define record_var_delete(x) _record_var_del(#x, x)



class Node {
public:
	int id;
	bool bInited;

	NodeGroupHost* host;
	NodeGroup* node_group;
	Thread* thread;

	std::string nodeclass;

	NodeInfo* infos;

private:
	bool bAttached;
	bool bFinished;

public:
	Node();
	virtual ~Node();

	virtual void init() = 0;
	virtual void process() = 0;
	virtual void on_receive(Message* m) = 0;
	virtual void on_request(const std::string& what, Message* out) {}

	long get_nb_outs();

	bool send(int iNeighbor, Message& m);

	inline bool send(int iNeighbor, int channel, const std::string& s) {
		Message m(0,0,channel,(const unsigned char*)s.c_str(),s.length()+1);
		return send(iNeighbor, m);
	}

	inline bool send(int iNeighbor, int channel, const unsigned char* data, size_t size) {
		Message m(0,0,channel,data,size);
		return send(iNeighbor, m);
	}

	std::string get_property(const std::string& key);
	float get_property_float(const std::string& key, float default_val);
	int get_property_int(const std::string& key, int default_val);
	bool has_property(const std::string& key);

	std::string get_group_name();
	std::string get_host_name();
	int get_thread();
	inline std::string get_desc() {return TOSTRING(get_group_name() << "_" << get_host_name() << "_" << id);}


	template <typename T> void _record_var_del(const std::string& name, const T& v) {
		std::string filename = TOSTRING(name << "_" << get_desc() << ".txt");
		//std::string filename = TOSTRING(home() << "/.agml/vars/" << name << "_" << get_desc() << ".txt");
		create_dir_for(filename);
		std::ofstream f(filename.c_str(), std::ofstream::out | std::ofstream::trunc);
		f.close();
	}

	template <typename T> void _record_var(const std::string& name, const T& v) {
		std::string filename = TOSTRING(name << "_" << get_desc() << ".txt");
		//std::string filename = TOSTRING(home() << "/.agml/vars/" << name << "_" << get_desc() << ".txt");
		create_dir_for(filename);
		std::ofstream f(filename.c_str(), std::ofstream::out | std::ofstream::app);
		f << v << "\n";
		f.close();
	}

	virtual std::string dump();


	///////////////
	// LIFECYCLE //
	///////////////

	void detach();
	void attach();
	void finish();


	void set_info_1(float val);
	void set_info_2(float val);



private:
	friend class Thread;
	friend class NodeGroup;
	INTERNAL void _init();
	INTERNAL void _process();
	INTERNAL void _receive(Message* m);
};



#endif /* NODE_NODE_H_ */
