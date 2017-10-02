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

#ifndef AGML_COMMANDS_H_
#define AGML_COMMANDS_H_

#include "com.h"

////////////////////////////
// USER TERMINAL COMMANDS //
////////////////////////////

/** Exit the AGML Server Deamon */
void agml_command_exit(Host* host, const char* params, size_t n);

/** Print a message */
void agml_command_echo(Host* host,const char* msg, size_t n);

/** Execute an OS shell command */
void agml_command_shell(Host* host, const char* cmd, size_t n);

/** Dump hosts connected to the target machine */
void agml_command_dump(Host* host, const char* params, size_t n); /** Returned back to the caller */
void agml_command_localdump(Host* host, const char* params, size_t n); /** Displayed locally on the target machine */

/** Request to enter the computing network (through a given bootstrap peer) */
void agml_command_enter(Host* host, const char* params, size_t n);

/** Request to leave the computing network */
void agml_command_leave(Host* host, const char* params, size_t n);


/////////////////////////////////
// NETWORK MANAGEMENT COMMANDS //
/////////////////////////////////

/** Request from a new peer to join the network */
void agml_command_subscribe(Host* host, const char* params, size_t n);

/** Request from a new peer to integrate you the network */
void agml_command_add_to_network(Host* host, const char* params, size_t n);

/** Forward a subscription request (e.g. to the root host) */
void agml_command_notify_subscription(Host* host, const char* params, size_t n);

/** Tell other hosts that we are the root host */
void agml_command_root_welcome(Host* host, const char* params, size_t n);

/** Request a Model */
void agml_command_model(Host* host, const char* params, size_t n);


//////////////////
// DATA STREAMS //
//////////////////

/** Realize a Topology */
void agml_command_set_topology(Host* host, const char* params, size_t n);

/** Connects a new data host */
void agml_command_data_host(Host* h, const char* params, size_t n);


///////////
// INFOS //
///////////


/** Request all nodes to start sending Infos */
void agml_command_start_infos(Host* h, const char* params, size_t n);

/** Request to start sending Infos */
void agml_command_do_start_infos(Host* h, const char* params, size_t n);

/** Command from DataHosts to root to update its local copy of all hosts infos */
void agml_command_update_infos(Host* h, const char* params, size_t n);

/** Ask for all infos as JSON */
void agml_command_infos(Host* h, const char* params, size_t n);

/** Response of agml_command_infos() */
void agml_command_infos_reply(Host* h, const char* params, size_t n);
void agml_process_infos_reply(const unsigned char* data, size_t size);



///////////////////
// NODE REQUESTS //
///////////////////

void agml_command_node_request(Host* h, const char* params, size_t n);


#endif /* AGML_COMMANDS_H_ */
