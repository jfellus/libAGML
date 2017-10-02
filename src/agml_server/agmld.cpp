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
/** 
 * @file agmld.cpp
 * @author Jerome Fellus
 * @date 2015
 * @copyright CNRS
 */


#include "../libagml_comm/server/Server.h"
#include "../libagml_comm/tcp/Socket.h"
#include "../libagml_comm/common/com.h"



static unsigned short PORT = 10001;
static std::string BOOTSTRAP_PEER_IP;
static std::string MODEL;

extern std::string SERVER_IP;
extern unsigned short SERVER_PORT;

bool is_me(const std::string& ip) {
	if(ip.find(':')!=std::string::npos)
		return resolve_ip(str_before(ip, ":"))==resolve_ip(SERVER_IP) && TOINT(str_after(ip, ":"))==SERVER_PORT;
	else return resolve_ip(BOOTSTRAP_PEER_IP)==resolve_ip(SERVER_IP) && SERVER_PORT==PORT;
}

/**
 * Entry point for the AGML Server Deamon.
 */
int main(int argc, char **argv) {
	try {
		bool bForcePort = false;
		if(argc>=2 && !strcmp(argv[1], "-p")) {
			if(argc>=3)	PORT = TOINT(argv[2]);
			else {DBG("option -p requires a port number"); exit(1);}
			argc-=2; argv+=2;
			bForcePort = true;
		}
		if(argc>=2 && strcmp(argv[1], "model")) {
			BOOTSTRAP_PEER_IP = argv[1];
			argc--; argv++;
		}
		if(argc>=3 && !strcmp(argv[1], "model")) {
			MODEL = argv[2];
		}

		if(bForcePort) server_start(PORT);
		else for(;PORT<10100; PORT++) {
			try {server_start(PORT); break; } catch(...) {}
		}
		if(!BOOTSTRAP_PEER_IP.empty() && !is_me(BOOTSTRAP_PEER_IP)) server_enter_network(BOOTSTRAP_PEER_IP);
		if(!MODEL.empty()) com_model(MODEL);
		server_join();
		DBG("AGML Server stopped");
	} catch(std::exception& s) {
		DBG("ERROR : " << s.what());
	}
}
