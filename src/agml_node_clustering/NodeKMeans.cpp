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

#include <float.h>
#include "NodeEM.cpp"


class NodeKMeans : public NodeEM {
public:

	uint K;

	Matrix codebook;

	float MSE;

	float epsilon;

protected:
	Matrix S;
	Matrix w;
	Matrix S_new;
	Matrix w_new;
	float s_MSE;
	float s_MSE_new;
	float w0;

	float MSE_old;
	float epsilon_t;
	float tepsilon;

public:

	virtual void init() {
		record_var_delete(MSE);
		MSE = FLT_MAX;
		epsilon = get_property_float("epsilon",0);
		tepsilon = 0;
		epsilon_t = 100;
		NodeEM::init();

		if(D>0 && !codebook) {
			K = get_property_int("K", 64);
			DBGV(D);
			DBGV(n);
			DBGV(K);

			codebook.init(K,D);
			S.init(K,D); w.init(K);
			S_new.init(K,D); w_new.init(K);

			w0 = n;

			init_codebook();
		}
	}

	virtual void init_codebook() {
		S = 0; w = 0;
		S_new = 0; w_new = 0;
		for(uint i=0; i<n; i++) {
			int argmin = rand()%K;
			S_new.row(argmin) += X.row(i);
			w_new[argmin]++;
		}

		s_MSE = s_MSE_new = 0;	 S += S_new; w += w_new;
	}

	void compute_codebook() {
		MSE_old = MSE;
		for(uint i=0; i<K; i++) {
			//if(w[i]<0.000001) continue;
			codebook.row(i) = S.row(i);
			codebook.row(i) /= w[i];
		}
		MSE = s_MSE / w0;

		set_info_1(MSE);
		if(verbose >= 1) DBG("Node " << id << "@" << get_host_name() << "\tthread = " << get_thread() << "\tMSE = " << MSE << "\t\t local iteration = " << nb_E_step);

		record_var(MSE);
		if(epsilon>0) check_termination();
	}

	void check_termination() {
		if(MSE_old - MSE < epsilon) tepsilon++; else tepsilon=0;
		if(tepsilon>=epsilon_t) {
			Message m(AGML_FINISH);
			for(int i = 0; i<get_nb_outs(); i++) send(i, m);
			the_end();
		}
	}

	void the_end() {
		DBG("Node " << id << "@" << get_host_name() << " finished");
		codebook.write(TOSTRING(get_group_name() << "@" << get_host_name() << "_" << id << ".fvec"));
		finish();
	}

	void save_codebook() {
		std::string file = TOSTRING("/run/shm/agml/" << get_group_name() << "@" << get_host_name() << "_" << id << ".fvec");
		create_dir_for(file);
		codebook.write(file);
	}

	virtual float dist(const Matrix& v1, const Matrix& v2) {
		return v1.l2p2(v2);
	}

	virtual void E_step() {
		if(!X || !codebook) return;

		compute_codebook();

		s_MSE -= s_MSE_new;	 S -= S_new; w -= w_new;
		s_MSE_new = 0; S_new = 0; w_new = 0;


		for(uint i=0; i<n; i++) {
			float min_e = FLT_MAX;
			uint argmin = 0;
			for(uint k=0; k<K; k++) {
				float e = dist(X.row(i),codebook.row(k));
				if(e <= min_e) { min_e = e; argmin = k; }
			}

			S_new.row(argmin) += X.row(i);
			w_new[argmin]++;
			s_MSE_new += min_e;
		}

		s_MSE += s_MSE_new;	 S += S_new; w += w_new;
	}

	virtual void M_step() {
		if(get_nb_outs()==0 || !S) return;
		if(w0<0.00001) return;

		S /= 2; w /= 2; s_MSE /= 2; w0 /= 2;

		Message m(AGML_CHANNEL_CODEBOOK);
		message_add_matrix(m,S);
		message_add_matrix(m,w);
		m.add(s_MSE);
		m.add(w0);

		int i = rand()%get_nb_outs(); // get_rand_neighbor();
		if(!send(i, m)) {
			S *= 2; w *= 2; s_MSE *= 2; w0 *= 2;
		}
	}

	virtual void on_receive(Message* m) {
		if(m->channel == AGML_CHANNEL_CODEBOOK) {
			Matrix S_in, w_in;
			message_get_matrix(m, S_in);
			message_get_matrix(m, w_in);

			float s_MSE_in = m->get<float>();
			float w0_in = m->get<float>();

			s_MSE += s_MSE_in;
			S += S_in;
			w += w_in;
			w0 += w0_in;
		} else if(m->channel == AGML_FINISH) {
			the_end();
		} else NodeEM::on_receive(m);
	}

	virtual void on_request(const std::string& what, Message* m) {
		if(what=="save") {
			save_codebook();
			m->add(TOSTRING("/run/shm/agml/" << get_group_name() << "@" << get_host_name() << "_" << id << ".fvec"));
		}
	}

};

AGML_NODE_CLASS(NodeKMeans)
