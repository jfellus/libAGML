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

#include "Server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "Socket.h"
#include <netinet/in.h>
#include "../util/utils.h"
#include <stdexcept>

struct ConnectionCallbackData {
	Socket* socket;
	void (*callback)(Socket*);
};


static void* _runServer(void* s) {
	((Server*)s)->run();
	return 0;
}

static void* _onConnect(void* s) {
	struct ConnectionCallbackData* ccd = (struct ConnectionCallbackData*)s;
	ccd->callback(ccd->socket);
	return 0;
}


Server::Server(int port, void(*connectionCallback)(Socket*)) {
	readBlocking = true; readTimeout = 0;
	this->connectionCallback = connectionCallback;
	if(port==0) port = DEFAULT_PORT;
	bClosed = false;

	// Create socket
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) throw std::runtime_error("Can't create server socket");

    // Create socket address
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    char yes = 1;
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));


    // Bind socket to address
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
             throw std::runtime_error(TOSTRING("Can't open server at port " << port));
    listen(sockfd,5);


    // Launch server thread
    pthread_create(&serverThread, NULL, _runServer, this);
}

Server::~Server() {
}

void Server::close() {
	bClosed = true;
	//pthread_cancel(serverThread);
	shutdown(sockfd, SHUT_RDWR);
	::close(sockfd);
}

void Server::join() {
	pthread_join(serverThread, NULL);
}

void Server::run() {
    struct sockaddr_in cli_addr;
    socklen_t clilen = sizeof(cli_addr);

    while(!bClosed) {
    	int newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    	if (newsockfd < 0) break;
    	pthread_t connectionThread;

    	struct ConnectionCallbackData* d = new struct ConnectionCallbackData;
    	d->socket = new Socket(newsockfd, &cli_addr, clilen);
    	d->socket->setReadBlocking(readBlocking);
    	if(!readBlocking) d->socket->setReadTimeout(readTimeout);
    	d->callback = connectionCallback;

    	pthread_create(&connectionThread, NULL, _onConnect, d);
    }
}
