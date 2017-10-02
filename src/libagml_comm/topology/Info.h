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

#ifndef INFO_H_
#define INFO_H_

#include "../util/utils.h"

class NodeInfo {
public:
	float nbProcess,nbSend,nbRecv;
	float ips, Ko_s, Ko_r;

	bool bAttached;

	float var1,var2;


	long nbprocess_second, Ko_second, Ko_r_second;
	long lasttime;

public:

	NodeInfo() { init(); }

	void init() {
		nbProcess = nbRecv = nbSend = 0;
		ips = Ko_s = Ko_r = 0;
		lasttime = get_time_ms();
		nbprocess_second = Ko_r_second = Ko_second = 0;
		bAttached = true;
	}

	void dump_JSON();
};


class NodeGroupHostInfo {
public:
	float nbProcess,nbSend,nbRecv;
	float ips, Ko_s, Ko_r;

	float moy,var;


	long nbprocess_second, Ko_second, Ko_r_second;
	long lasttime;


	NodeGroupHostInfo() { init(); }

	void init() {
		nbProcess = nbRecv = nbSend = 0;
		ips = Ko_s = Ko_r = 0;
		lasttime = get_time_ms();
		nbprocess_second = Ko_r_second = Ko_second = 0;
	}

	void update();

	void dump_JSON();
};


void agml_infos_start_update_thread(const std::string& params);


#endif /* INFO_H_ */
