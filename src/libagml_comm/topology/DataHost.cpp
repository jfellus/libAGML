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

#include "DataHost.h"
#include <stdexcept>
#include <map>

array<DataHost*> data_hosts;



DataHost::DataHost(const std::string& host_name, const std::string& server_ip, int nb_threads): nb_threads(nb_threads) {
	host = NULL;
	if(agml_get_datahost(host_name)!=NULL) throw std::runtime_error(TOSTRING("A DataHost with the same host_name already exists : " << host_name));

	this->server_ip = server_ip;
	this->host_name = host_name;

	data_hosts.add(this);
}

DataHost::~DataHost() {
	data_hosts.remove(this);
}

void DataHost::connect() {
	if(is_local() || is_connected()) return;
	host = new Host(new Socket(server_ip.c_str()),false);
	host->send_sys_command("data_host", agml_get_first_local_datahost()->host_name);
}

DataHost* agml_get_datahost(const std::string host_name) {
	for(uint i=0; i<data_hosts.size(); i++) {
		if(data_hosts[i]->host_name == host_name) return data_hosts[i];
	}
	return NULL;
}

DataHost* agml_get_first_local_datahost() {
	for(uint i=0; i<data_hosts.size(); i++) {
		if(data_hosts[i]->is_local()) return data_hosts[i];
	}
	return NULL;
}



