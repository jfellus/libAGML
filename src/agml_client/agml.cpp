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
 * @file agml.cpp
 * @author Jerome Fellus
 * @date 2015
 * @copyright CNRS
 */



#include "../libagml_comm/util/utils.h"
#include "../libagml_comm/common/com.h"
#include "../libagml_comm/common/Commands.h"
#include "../libagml_comm/client/Client.h"

static std::string server_ip = "localhost:10001";
static std::string command = "";
static std::string command_args = "";

Client* client = NULL;





int run_client() {
	client = new Client(server_ip);

//	DBG("agml> " << command << " " << command_args);
	client->send_sys_command(command,command_args);

	if(command=="infos") {
		Message m; m.read(client->socket);
		MessageElt me = m.get_next();
		agml_process_infos_reply(me.data, me.size);
	}
	else if(AGML_COMMAND_HAS_RESPONSE(command)) {
//		char res[10000];
//		if(client->socket->read(res, 10000)>0) {
//			DBG(res);
//		} else ERROR("Error");
		Message m; m.read(client->socket);
		MessageElt me = m.get_next();
		DBG((const char*)me.data);
	}

	delete client;
	return 0;
}



/**
 * Entry point for AGML Client commands
 */
int main(int argc, char **argv) {
	if(argc>=2) command = argv[1];
	if(argc>=2 && AGML_COMMAND_ID(command)==-1) {
		server_ip = argv[1];
		if(argc>=3) {
			command = argv[2];
			for(int i=3; i<argc; i++) {
				command_args += argv[i]; if(i<argc-1) command_args += " ";
			}
		} else {
			command = "echo";
			command_args = "hello";
		}
	} else {
		if(argc>=2) {
			command = argv[1];
			for(int i=2; i<argc; i++) {
				command_args += argv[i]; if(i<argc-1) command_args += " ";
			}
		} else {
			command = "echo";
			command_args = "hello";
		}
	}

	if(command=="") { DBG("Nothing to do"); return 0;}

	try {return run_client();}
	catch(const char* s) {	ERROR("ERROR : " << s);	return 1;	}
	catch(std::string& s) {	ERROR("ERROR : " << s);		return 1;	}
	catch(std::exception& e) {	ERROR("ERROR : " << e.what());		return 1;	}
	catch(...) { ERROR("FATAL ERROR"); return 1;}
}
