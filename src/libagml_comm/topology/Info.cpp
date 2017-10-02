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

#include "Info.h"
#include <iostream>
#include <map>
#include "../common/com.h"
#include "../topology/Topology.h"

void NodeInfo::dump_JSON() {
	std::cout << "{ ";
	std::cout << "\"nbProcess\" : " << nbProcess << ", ";
	std::cout << "\"nbSend\" : " << nbSend << ", ";
	std::cout << "\"nbRecv\" : " << nbRecv << ", ";
	std::cout << "\"ips\" : " << ips << ", ";
	std::cout << "\"Ko_s\" : " << Ko_s << ", ";
	std::cout << "\"Ko_r\" : " << Ko_r << ", ";
	std::cout << "\"bAttached\" : " << bAttached << ", ";
	std::cout << "\"moy\" : " << var1 << ", ";
	std::cout << "\"var\" : " << var2;
	std::cout << " }";
}


void NodeGroupHostInfo::update() {
	long t = get_time_ms();
	if(t-lasttime > 1000) {
		ips = ((int)(nbprocess_second)*10/((t-lasttime)/1000))/10.0;
		Ko_s = ((long)(Ko_second*10)/((t-lasttime)))/10.0;
		Ko_r = ((long)(Ko_r_second*10)/((t-lasttime)))/10.0;
		lasttime = t;
		nbprocess_second = 0;
		Ko_second = 0;
		Ko_r_second = 0;
	}
}

void NodeGroupHostInfo::dump_JSON() {
	std::cout << "{ ";
	std::cout << "\"nbProcess\" : " << nbProcess << ", ";
	std::cout << "\"nbSend\" : " << nbSend << ", ";
	std::cout << "\"nbRecv\" : " << nbRecv << ", ";
	std::cout << "\"ips\" : " << ips << ", ";
	std::cout << "\"Ko_s\" : " << Ko_s << ", ";
	std::cout << "\"Ko_r\" : " << Ko_r << ", ";
	std::cout << "\"moy\" : " << moy << ", ";
	std::cout << "\"var\" : " << var;
	std::cout << " }";
}

///////////////////////



void* agml_infos_update_thread(void*) {
	while(true) {
		if(root_host) {
			Message m; m.set_command("update_infos");
			Topology::cur->dump_local_infos(&m);
			root_host->send(&m);
		}
		sleep(1);
	}
	return 0;
}

static bool b_agml_infos_update_started = false;
pthread_t agml_infos_update_th;
void agml_infos_start_update_thread(const std::string& params) {
	if(b_agml_infos_update_started) return;
	b_agml_infos_update_started = true;
	pthread_create(&agml_infos_update_th, NULL, agml_infos_update_thread, NULL);
}
