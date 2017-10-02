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

#include "Thread.h"
#include "../topology/Topology.h"
#include <iomanip>
#include <semaphore.h>

static int _cur_id = 0;

Thread::Thread() {
	this->id = _cur_id++;
	this->thread = 0;
	bStopped = true;
	bHasThread = false;
	nb_running_nodes = 0;
	bRunning = false;
}

Thread::~Thread() {
}




///////////////
// ACCESSORS //
///////////////


Node* Thread::get_node(long node_id) {
	return nodes[node_id];
}

void Thread::add(Node* node) {
	nb_running_nodes++;
	node->attach();
}

void Thread::remove(Node* node) {
	nb_running_nodes--;
	if(nb_running_nodes==0) bRunning = false;
	node->detach();
}

void Thread::attach(Node* node) {
	nodes.add(node);
	sem_post(&sem);
}

void Thread::detach(Node* node) {
	nodes.remove(node);
	sem_post(&sem);
}


///////////////
// LIFECYCLE //
///////////////

void* _start_thread (void* p) { long r = ((Thread*)p)->run(); return (void*)r;}

void Thread::start() {
	if(!bStopped) {	ERROR("Thread " << id << " already started");	return;	}
	bStopped = false;
	if(!bHasThread) {pthread_create(&thread, NULL, _start_thread, this); bHasThread = true;}
	sem_post(&sem);
}

void Thread::stop() {
	if(bStopped) {	ERROR("Thread " << id << " already stopped");	return;	}
	bStopped = true;
}


/** Main execution loop  */
int Thread::run() {
	bRunning = true;
	long nbprocessed = 0;
	long lasttime = get_time_ms();
	while(bRunning) {
		while(bStopped || (nb_nodes()==0 && fifo.empty())) sem_wait(&sem);


		// Pull any pending message
		while(!fifo.empty()) {
			Message* m = fifo.pop();
			on_receive(m);
			delete m;
			m = 0;
		}

		if(nb_nodes() > 0) {
			Node* n = draw_random_node();
			if(n->bFinished) continue;

			if(!n->bInited) {
				n->bInited = true;
				n->_init();
			}
			if(n->bAttached) {
				n->_process();
				nbprocessed++;
			}
		}


		long time = get_time_ms();
		if(time-lasttime>=1000) {
			int n = nbprocessed*10000.0/(time-lasttime);
			DBG("Process " << std::fixed << std::setprecision(1) << n/10.0 << " nodes/s");
			nbprocessed = 0;
			lasttime = time;
		}
	}

	bHasThread = false;
	agml_thread_end(this);
	return 0;
}





//////////////////////
// NODES SCHEDULING //
//////////////////////


Node* Thread::draw_random_node() {
	return nodes[rand()%nodes.size()];
}





////////////////////
// COMMUNICATIONS //
////////////////////

void Thread::push_message(long src, long dst, int channel, const unsigned char* data, size_t size) {
	push_message(new Message(src, dst, channel, data, size));
}

void Thread::push_message(Message* m) {
	fifo.push(m);
	sem_post(&sem);
}

void Thread::on_receive(Message* m) {
	Node* n = com_decode_local_node(m->dst);
	if(!n) { ERROR("ERROR : Node overflow in thread " << id << " for node " << m->dst); throw std::runtime_error("node overflow"); }
	try {
		n->_receive(m);
	} catch(std::exception& e) { ERROR("ERROR while receiving at node " << n->dump() << " : " << e.what()); }
}



/////////////////////

void agml_threads_create(int nb_threads) {
	DBG("Create " << nb_threads << " simulation threads");
	for(int i=0; i<nb_threads; i++) threads.add(new Thread());
}

void agml_threads_start() {
	for(size_t i=0; i<threads.size(); i++) threads[i]->start();
}

void agml_thread_end(Thread* th) {
	threads.remove(th);
	if(threads.empty()) com_exit();
}

int threads_get_lightest() {
	int th = 0;
	long th_load = -1;
	for(uint i=0; i<threads.size(); i++) {
		if(threads[i]->nb_nodes() < th_load || th_load == -1) {
			th = i; th_load = threads[i]->nb_nodes();
		}
	}
	return th;
}


