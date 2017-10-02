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

class NodeData : public Node {
private:
	size_t n,D;
	Matrix X;

	int channel;

	bool* ack;
	size_t nback;

	uint verbose;
public:

	virtual Matrix generate_data() = 0;

	virtual void init() {
		verbose = get_property_int("verbose", 0);
		channel = get_property_int("channel", AGML_CHANNEL_TRAINING_DATA);

		ack = 0;
		nback = 0;
		if(!X) {
			X = generate_data();
			n = X.height;
			D = X.width;
			if(n==0 || D==0) AGML_FATAL_ERROR("NodeData : Empty data !");
		}
	}

	virtual void process() {
		if(get_nb_outs()==0) return;

		if(nback >= (size_t)get_nb_outs()) {
			if(verbose >= 1) DBG("Data sent ! done !");
			finish();
			return;
		}

		if(!ack) {
			ack = new bool[get_nb_outs()];
			memset(ack, 0, sizeof(bool)*get_nb_outs());
			nback = 0;
		}

		for(int i=0; i<get_nb_outs(); i++) {
			if(ack[i]) continue;
			Message m(channel);
			Matrix p = X.part(i, get_nb_outs());
			if(p) message_add_matrix(m, p);
			if(!p || send(i, m)) {
				nback++;
				ack[i] = true;
				if(verbose>=1) DBG("Sent " << (!p ? "empty" : "") << " data " << nback << "/" << get_nb_outs());
			}
		}
	}

	virtual void on_receive(Message* m) {}
};
