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


#ifndef NodeEM_H_
#define NodeEM_H_


#include <agml/node.h>
#include "../agml/channels.h"

class NodeEM : public Node {
public:
	std::string path; // Path for the training set
	Matrix X; 	// Training dataset
	int M; 		// Number of sends between each E-step

	uint D; // Dimension of the training vectors
	uint n; // Number of traning vectors

	uint nb_E_step;
	uint nb_M_step;

	uint verbose;

private:
	int m;

public:

	virtual void init() {
		verbose = get_property_int("verbose",0);
		M = get_property_int("M", 10);
		if(X) {
			nb_E_step = nb_M_step = 0;

			D = X.width;
			n = X.height;
			attach();
		} else detach();
	}

	virtual void E_step() {	}
	virtual void M_step() {	}

	virtual void process() {
		if(!X) return;

		if(m>=M || get_nb_outs()==0) {	E_step();	nb_E_step++; m = 0; }
		else {		M_step();	nb_M_step++; m++;}
	}

	virtual void on_receive(Message* m) {
		if(m->channel == AGML_CHANNEL_TRAINING_DATA) {
			message_get_matrix(m,X);
			init();
		}
	}
};


#endif
