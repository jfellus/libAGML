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

#ifndef TOPOLOGYREADER_H_
#define TOPOLOGYREADER_H_

#include "Topology.h"


class TopologyReader {
public:
	Topology* topology;
	std::istream& s;

	int default_nb_threads;

	TopologyReader(std::istream& s) : s(s) {
		default_nb_threads = 1;
		topology = NULL;
	}

	void read(Topology* t);

	void read_hosts();

	void execute_statement(const std::string& statement);

private:
	void execute_statement_declare_host(const std::string& statement);
	void execute_statement_connect(std::string& src, std::string& dst, bool bAllowSelf = false, bool bOnlyLocal = false);
	void execute_statement_add_group(std::string& group, std::string& node_type);
	void execute_statement_group_set(std::string& group, std::string& key, std::string& val);
	void execute_statement_assign(std::string& group, std::string& host, std::string& nb);

	DataHost* parse_declare_host(const std::string& statement);
};


#endif /* TOPOLOGYREADER_H_ */
