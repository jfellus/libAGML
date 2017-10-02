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



#include <agml/node.h>
#include "channels.h"

class NodeAvg : public Node {
public:
	Matrix X;
	size_t n,D;

	Matrix S;
	float w;
public:

	virtual void init_sum_weight(size_t n, size_t D) {
		this->D = D;
		this->n = n;
		S.init(n,D);
		S = 0; w = 0;
		attach();
	}

	virtual void init() {
		if(X) {
			if(!S) init_sum_weight(X.height, X.width);
			S += X;
			w += 1;
		} else detach();
	}

	void update() {
		X = (const Matrix&) S;
		X /= w;
		set_info_1(X[0]);
		SYNC_START();
		X.dump();
		usleep(8000);
		SYNC_END();
	}

	virtual void process() {
		if(get_nb_outs()>0){
			int i = rand()%get_nb_outs();

			w/=2;S/=2;

			Message m(AGML_CHANNEL_GRADIENT);
			message_add_matrix(m, S);
			m.add(w);
			if(!send(i, m)) {
				w*=2; S*=2;
			}
		}
		usleep(10000);
	}

	virtual void on_receive(Message* m) {
		if(m->channel==AGML_CHANNEL_TRAINING_DATA) {
			message_get_matrix(m, X);
			init();
		} else if(m->channel==AGML_CHANNEL_GRADIENT) {
			Matrix Sin;
			message_get_matrix(m, Sin);
			float win = m->get<float>();

			if(!S) init_sum_weight(Sin.height, Sin.width);

			S += Sin;
			w += win;

			update();
		}
	}
};

AGML_NODE_CLASS(NodeAvg)
