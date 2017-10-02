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

class NodeTestMatrix : public Node {
public:
	Matrix X;

	Matrix Xinv;
public:

	virtual void init() {

		DBG("----------- BASIC MANIPULATIONS ---------------");
		X.init(3,3);
		X(0,0) = 1; X(0,1) = 2; X(0,2) = 3;
		X(1,0) = 0; X(1,1) = 1; X(1,2) = 4;
		X(2,0) = 5; X(2,1) = 6; X(2,2) = 0;

		Xinv.init(3,3);
		Xinv(0,0) = -24; Xinv(0,1) = 18; Xinv(0,2) = 5;
		Xinv(1,0) = 20; Xinv(1,1) = -15; Xinv(1,2) = -4;
		Xinv(2,0) = -5; Xinv(2,1) = 4; Xinv(2,2) = 1;

		X.dump();
		X.diag().dump();

		X /= 2;
		X.dump();
		X *= 4;
		X.dump();

		DBG("-------------- INV -----------------------");

		Matrix Y = X.inv();

		Y.dump();
		Xinv.dump();

		DBGV(Y.l2(Xinv));

		Matrix B = X*Y;
		B.dump();

		DBG("---------------- QR --------------------");
		Matrix Q,R;
		X.qr(&Q, &R);
		Q.dump();
		R.dump();
		Matrix Z = Q*R;
		X.dump();
		Z.dump();

		Q.correlation().dump();

		DBG("------------ GRAM/CORRELATION -----------");
		Matrix U("1 2 3 4 ; 0 0 1 0 ; 1 1 1 1");
		U.dump();
		U.gram().dump();
		U.correlation().dump();


		DBG("------------  EIG -------------");

		DBG("1) Full");
		Matrix ee("-4 2 -2 ; 2 -7 4 ; -2 4 -7");
		DBG("A="); ee.dump();

		Matrix V,L;
		Matrix r;
		ee.eig_sym(&V,&L);
		DBG("EIG=");
		L.dump();
		V.dump();
		DBG("ULUt=");
		r.reconstruct(V,L);r.dump();

		DBG("Diagonalization : ");
		(V.transpose()*ee*V).dump();

		DBG("2) Rank deficient (r=3)");
		ee.clear();
		V.clear(); L.clear();
		V = std::string("[ 10 1 2 3 4 ; 5 -2 3 5 -8 ; 1 0 1 0 1 ]");
		L = std::string("[ 1 ; 0.5 ; 1.2 ]");
		ee.reconstruct(V,L);
		DBG("A=");
		ee.dump();
		V.clear(); L.clear();
		ee.eig_sym(&V, &L, ee.width);
		DBG("Eig = ");
		V.dump();
		L.dump();

		DBG(" full reconstruction : ");
		r.reconstruct(V,L);r.dump();
		DBG("L2=" << ee.l2p2(r));

		for(int d = 4; d>0; d--) {
			DBG(" r4 reconstruction : ");
			r.reconstruct(V,L.trim_width(d));r.dump();
			DBG("L2=" << ee.l2p2(r));
		}


		DBG("-------------- QR ITERATION -------------");


		Matrix AA(10,10); AA.randf(0,5);
		AA += AA.transpose();
		Matrix BB(10,10); BB.randf(0,5);
		BB += BB.transpose();
		Matrix CC(10,10);
		for(size_t i=0; i<CC.n; i++) CC[i] = 0.5f*(AA[i] + BB[i]);

		DBG("A="); AA.dump();
		DBG("B="); BB.dump();
		DBG("C="); CC.dump();

		Matrix AU,AL; AA.eig_sym(&AU,&AL);
		Matrix BU,BL; BB.eig_sym(&BU,&BL);
		Matrix CU,CL; CC.eig_sym(&CU,&CL);

		DBG("DO");
		eigendecompose_sum(AU,AL, BU, BL);
		DBG("OK");
		Matrix Crec;
		Crec.reconstruct(AU,AL);
		DBG("Crec="); Crec.dump();



		exit(0);
	}

	void eigendecompose_sum(Matrix& U1, Matrix& L1, const Matrix& U2, const Matrix& L2) {
		Matrix Q(U1.height,U1.width); Q = (const Matrix&) U1;
		Matrix R(1,L1.width);
		Matrix M(Q.height,Q.width);

		for(int t=0; t<1000; t++) {
			for(size_t i = 0; i < M.height; i++) {
				for(size_t j = 0; j < M.width; j++) {
					M(i,j) = 0;
					for(size_t k = 0; k < L1.width; k++) {
						float v1 = 0;
						float v2 = 0;
						for(size_t m = 0; m < Q.height; m++) {
							v1 += Q(m,j)*U1(m,k);
							v2 += Q(m,j)*U2(m,k);
						}
						M(i,j) += L1[k] * U1(i,k) * v1;
						M(i,j) += L2[k] * U2(i,k) * v2;
					}
				}
			}
			M.qr(&Q,&R);
//			R.diag().dump();
			//usleep(50000);
		}

		U1.clear(); L1.clear();
		U1 = (const Matrix&) Q;
		L1 = (const Matrix&) R.diag();
	}

	virtual void process() {
	}

	virtual void on_receive(Message* m) {
	}
};

AGML_NODE_CLASS(NodeTestMatrix)
