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

#include "TopologyReader.h"
#include "NodeLibrary.h"

extern array<DataHost*> data_hosts;



void TopologyReader::read(Topology* t) {
	topology = t;

	std::string line;
	while(s.good() && getline(s, line)) {
		line = str_trim(str_before(line, "#"));
		if(line.empty()) continue;
		else execute_statement(line);
	}
}

void TopologyReader::read_hosts() {
	std::string line;
	while(s.good() && getline(s, line)) {
		line = str_trim(str_before(line, "#"));
		if(line.empty()) continue;
		else if(str_starts_with(line, "Host ")) parse_declare_host(str_trim(str_after(line, "Host")));
	}
}



DataHost* TopologyReader::parse_declare_host(const std::string& statement) {
	std::string hostname = str_trim(str_before(statement, "="));
	DataHost* dh = agml_get_datahost(hostname);
	if(dh) return dh;
	dh = new DataHost(hostname);
	dh->server_ip = str_trim(str_after(statement, "="));
	dh->nb_threads = default_nb_threads;
	if(str_has(dh->server_ip, " ")) {
		dh->nb_threads = TOINT(str_trim(str_after(dh->server_ip, " ")));
		dh->server_ip = str_trim(str_before(dh->server_ip, " "));
	}
	return dh;
}

void TopologyReader::execute_statement_declare_host(const std::string& statement) {
	DataHost* dh = parse_declare_host(statement);
	if(dh->is_local()) agml_threads_create(dh->nb_threads);
}

void TopologyReader::execute_statement_connect(std::string& src, std::string& dst, bool bAllowSelf, bool bOnlyLocal) {
	topology->connect(src, dst, bAllowSelf, bOnlyLocal);
}

void TopologyReader::execute_statement_add_group(std::string& group, std::string& node_type) {
	topology->add_group(group, node_type);
}

void TopologyReader::execute_statement_group_set(std::string& group, std::string& key, std::string& val) {
	topology->set_group_property(group, key, val);
}



void TopologyReader::execute_statement_assign(std::string& group, std::string& host, std::string& nb) {
	int thread = -2; // -2 = evenly distribute
	int nb_nodes = 1;

	// "Same as" assignment
	if(host[0]=='=') {
		topology->assign_same_as(group, str_trim(host.substr(1)), nb.empty() ? -1 : TOINT(nb));
		return;
	}

	if(str_has(host, " ")) {
		std::string sthread = str_trim(str_after(host, " "));
		thread = sthread=="*" ? -1 : TOINT(sthread);
		host = str_trim(str_before(host, " "));
	}

	if(!nb.empty()) nb_nodes = TOINT(nb);

	// Allocate hosts
	if(host=="*") {
		for(uint i = 0; i<data_hosts.size(); i++) {
			topology->assign(group, data_hosts[i]->host_name, thread==-1 ? nb_nodes*data_hosts[i]->nb_threads : nb_nodes, thread<0 ? -1 : thread);
		}
	}
	else {
		DataHost* dh = agml_get_datahost(host);
		topology->assign(group, host, thread==-1 ? nb_nodes*dh->nb_threads : nb_nodes, thread<0 ? -1 : thread);
	}
}



void TopologyReader::execute_statement(const std::string& statement) {
	if(str_starts_with(statement, "Host ")) execute_statement_declare_host(str_trim(str_after(statement, "Host")));
	else if(str_has(statement, "->")) {
		std::string src = str_trim(str_before(statement, "->"));
		std::string dst = str_trim(str_after(statement, "->"));
		execute_statement_connect(src,dst, false);
	} else if(str_has(statement, "-o>")) {
		std::string src = str_trim(str_before(statement, "-o>"));
		std::string dst = str_trim(str_after(statement, "-o>"));
		execute_statement_connect(src,dst, true);
	} else if(str_has(statement, "-L>")) {
		std::string src = str_trim(str_before(statement, "-L>"));
		std::string dst = str_trim(str_after(statement, "-L>"));
		execute_statement_connect(src,dst, false, true);
	}
	else if(str_has(statement, "@")) {
		std::string group = str_trim(str_before(statement, "@"));
		std::string host = str_trim(str_between(statement, "@", "["));
		std::string nb = str_has(statement, "[") ? str_trim(str_between(statement, "[", "]")) : "";
		execute_statement_assign(group, host, nb);
	} else if(str_has(statement, "=") && str_has(statement, ".")) {
		std::string group = str_trim(str_before(statement, "="));
		std::string val = str_trim(str_after(statement, "="));
		std::string key = str_trim(str_after(group, "."));
		group = str_trim(str_before(group, "."));
		execute_statement_group_set(group, key, val);
	} else if(str_has(statement, " ")) {
		std::string nodeclass = str_trim(str_before(statement, " "));
		std::string group = str_trim(str_after(statement, " "));
		execute_statement_add_group(group, nodeclass);
	} else throw std::runtime_error(TOSTRING("Unknown statement : " << statement));
}


