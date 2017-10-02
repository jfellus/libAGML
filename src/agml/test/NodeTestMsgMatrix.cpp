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


#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <agml/node.h>
#include <iostream>
#include <util/utils.h>
#include "../channels.h"
#include "../math/Matrix.h"
#include "../com/message.h"

class NodeTestMsgMatrix : public Node {
public:
	Matrix X;
	float idd;
public:
	virtual void init() {
		DBG(id << " INIT");
		idd = id;
		X.init(3,10);
		for(size_t i=0; i<X.height; i++) {
			for(size_t j=0; j<X.width; j++) {
				X(i,j) = i*j + id;
			}
		}
		X.dump();

		Matrix Y = X.gram();
		DBG("------------");
		Y.dump();
		exit(0);
		sleep(1);
	}

	virtual void process() {
		DBG(id << " SEND");
		X.dump();
		DBG(idd);
		if(get_nb_outs()>0){
			int i = rand()%get_nb_outs();
			Message m(AGML_CHANNEL_CODEBOOK);
			message_add_matrix(m, X);
			m.add(idd);
			send(i, m);
		}
		sleep(1);
	}

	virtual void on_receive(Message* m) {
		DBG(id << " RECV");
		Matrix X;
		message_get_matrix(m,X);
		float iddd = m->get<float>();
		X.dump();
		DBG(iddd);
		sleep(1);
	}
};

AGML_NODE_CLASS(NodeTestMsgMatrix)
