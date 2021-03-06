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
#include <float.h>

class NodeEvalKmeans : public Node {
public:

	std::string file;
	Matrix X;
	size_t n,D;

public:

	virtual void init() {
		detach();
		if(!X) {
			if(has_property("file")) {
				file = get_property("file");
				X.read(file);
			}
		}
		D = X.width;
		n = X.height;
	}

	virtual void process() {}


	virtual float dist(const Matrix& v1, const Matrix& v2) {
		return v1.l2p2(v2);
	}


	void eval(const Matrix& codebook) {
		float MSE = 0;
		for(uint i=0; i<n; i++) {
			float min_e = FLT_MAX;
			for(uint k=0; k<codebook.height; k++) {
				float e = dist(X.row(i),codebook.row(k));
				if(e <= min_e) min_e = e;
			}
			MSE += min_e;
		}
		MSE /= n;
		DBGV(MSE);
	}

	virtual void on_receive(Message* m) {
		if(m->channel == AGML_CHANNEL_TRAINING_DATA && !X) {
			message_get_matrix(m, X);
			init();
		} else if(m->channel == AGML_CHANNEL_CODEBOOK) {
			Matrix codebook;
			message_get_matrix(m, codebook);
			eval(codebook);
		}
	}



};

AGML_NODE_CLASS(NodeEvalKmeans)
