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

#ifndef AGML_THREAD_H_
#define AGML_THREAD_H_

#include "../util/utils.h"
#include "../util/fifo.h"
#include "../common/com.h"
#include "../util/array.h"
#include <pthread.h>
#include "../common/Host.h"
#include "../agml/node.h"
#include <semaphore.h>

class Node;

class Thread {
public:
	int id;

	array<Node*> nodes;

	FIFO<Message*> fifo;

	bool bStopped, bHasThread;
	pthread_t thread;
	pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
	sem_t sem;

private:
	bool bRunning;
	uint nb_running_nodes;

public:
	Thread();
	virtual ~Thread();

	///////////////
	// ACCESSORS //
	///////////////

	void add(Node* node);
	void remove(Node* node);
	int nb_nodes() {return nodes.size();}
	Node* get_node(long node_id);




	///////////////
	// LIFECYCLE //
	///////////////

	void start();
	void stop();

	void LOCK() { pthread_mutex_lock(&mut);}
	void UNLOCK() {pthread_mutex_unlock(&mut);}

	INTERNAL int run();


	void attach(Node* node);
	void detach(Node* node);
	void on_node_finish();


	//////////////////////
	// NODES SCHEDULING //
	//////////////////////

	Node* draw_random_node();





	////////////////////
	// COMMUNICATIONS //
	////////////////////

	void push_message(Message* m);
	void push_message(long src, long dst, int channel, const unsigned char* data, size_t size);
	void on_receive(Message* m);

};


void agml_threads_create(int nb_threads);
void agml_threads_start();
void agml_thread_end(Thread* th);

int threads_get_lightest();

#endif /* AGML_THREAD_H_ */
