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

#include "Matrix.h"
#include <veccodec.h>
#include <algebra.h>


WidthTrimedMatrix Matrix::trim_width(uint w) {return WidthTrimedMatrix(*this, w);}
WidthTrimedMatrix::operator Matrix() {
	Matrix ret(m.height, w);
	for(size_t i=0; i<m.height; i++) memcpy(ret.data, &m.data[i*m.width], w*sizeof(float));
	return ret;
}

Matrix& Matrix::operator+=(const WidthTrimedMatrix& m) {
	for(size_t i=0; i<height; i++) {
		vector_add_float(&data[i*width], &m.m.data[i*m.m.width], m.w);
	}
	return *this;
}
Matrix& Matrix::operator-=(const WidthTrimedMatrix& m) {
	for(size_t i=0; i<height; i++) {
		vector_sub_float(&data[i*width], &m.m.data[i*m.m.width], m.w);
	}
	return *this;
}
Matrix& Matrix::operator=(const WidthTrimedMatrix& m) {
	if(!data) init(m.m.height, m.w);
	for(size_t i=0; i<m.m.height; i++) {
		memcpy(&data[i*width], &m.m.data[i*m.m.width], sizeof(float)*m.w);
	}
	return *this;
}



HeightTrimedMatrix Matrix::trim_height(uint w) {return HeightTrimedMatrix(*this, w);}
HeightTrimedMatrix::operator Matrix() {
	Matrix ret(h, m.width);
	memcpy(ret.data, &m.data, ret.n*sizeof(float));
	return ret;
}

Matrix& Matrix::operator+=(const HeightTrimedMatrix& m) {
	vector_add_float(data, m.m.data, m.h*m.m.width);
	return *this;
}
Matrix& Matrix::operator-=(const HeightTrimedMatrix& m) {
	vector_sub_float(data, m.m.data, m.h*m.m.width);
	return *this;
}
Matrix& Matrix::operator=(const HeightTrimedMatrix& m) {
	if(!data) init(m.h, m.m.width);
	memcpy(data, m.m.data, n*sizeof(float));
	return *this;
}



float Matrix::l2p2(const Matrix& m) const {return vector_l2p2_float(data, m.data, n); }
float Matrix::l2(const Matrix& m) const {return vector_l2_float(data, m.data, n); }
float Matrix::n2p2() const {return vector_n2p2_float(data, n); }
float Matrix::n2() const {return vector_n2_float(data, n); }

Matrix& Matrix::operator*=(float f) {vector_smul_float(data, f, n); return *this;}
Matrix& Matrix::operator/=(float f) {vector_smul_float(data, 1.0/f, n); return *this;}

Matrix& Matrix::operator+=(const Matrix& m) {
	if(width!=m.width || height!=m.height) throw std::runtime_error("Matrix dimensions must agree!");
	vector_add_float(data, m.data, n); return *this;
}
Matrix& Matrix::operator-=(const Matrix& m) {
	if(width!=m.width || height!=m.height) throw std::runtime_error("Matrix dimensions must agree!");
	vector_sub_float(data, m.data, n); return *this;
}

Matrix& Matrix::operator=(const std::string& s) {
	std::string s1 = str_trim(str_replace(str_replace(s, "]", " "), "[", " "));
	size_t rows = std::count(s1.begin(), s1.end(), ';');
	std::string first = str_trim(str_before(s1, ";"));
	size_t cols = std::count(first.begin(), first.end(), ' ');
	init(rows+1, cols+1);
	std::string ss = str_trim(str_replace(s1, ";", " "));
	std::istringstream iss(ss);
	for(size_t i=0; i<height; i++) {
		for(size_t j=0; j<width; j++) {
			iss >> data[i*width + j];
		}
	}
	return *this;
}


bool Matrix::read(const std::string& file) {
	veccodec_load_float(data, width, height, file.c_str());
	return false;
}

bool Matrix::write(const std::string& file) {
	veccodec_save_float(data, width, height, file.c_str());
	return false;
}

Matrix Matrix::col_sums() {
	Matrix m(1,width);
	m=0;
	for(size_t i=0; i<height; i++) {
		for(size_t j=0; j<width; j++) {
			m[j] += data[i*width+j];
		}
	}
	return m;
}

Matrix Matrix::row_sums() {
	Matrix m(height,1);
	m=0;
	for(size_t i=0; i<height; i++) {
		for(size_t j=0; j<width; j++) {
			m[i] += data[i*width+j];
		}
	}
	return m;
}

Matrix Matrix::gram() {
	Matrix g(height,height); g = 0;
	matrix_CpAtB_float(g.data, data, data, height, width, height);
	return g;
}

float Matrix::dot(const Matrix& m) const { return vector_ps_float(data, m.data, n); }


Matrix Matrix::operator*(const Matrix& m) {
	if(width!=m.height) throw std::runtime_error("Matrix dimensions must agree !");
	Matrix res(height,m.width);
	res = 0;
	matrix_CpAB_float(res, data, m.data, height, width, m.width);
	return res;
}

Matrix Matrix::inv() {
	if(!is_square()) throw std::runtime_error("Matrix must be square");
	Matrix m(height, width);
	matrix_inv2_float(m.data, data, height);
	return m;
}

Matrix Matrix::correlation() {
	Matrix c(width,width); c = 0;
	matrix_CpABt_float(c.data, data, data, width, height, width);
	return c;
}

Matrix& Matrix::scale(const Matrix& v) { for(size_t i=0; i<n; i++) vector_mul_float(&data[i*width],v.data,width); return *this;}


void Matrix::qr(Matrix* Q, Matrix* R) {
	Q->init(height, width);
	R->init(height,height); *R = 0;
	matrix_qr_thin_float(data, Q->data, R->data, height, width);
}

void Matrix::dump(bool bExact) {
	for(size_t i = 0; i<height; i++) {
		for(size_t j = 0; j<width; j++) {
			std::cout.precision(4);
			if(!bExact && data[i*width + j] > -0.00001 && data[i*width + j] < 0.00001) std::cout << "0 ";
			else std::cout << data[i*width + j] << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";
}

void Matrix::eig_sym(Matrix* U, Matrix* L) {
	if(!is_square()) throw std::runtime_error("Matrix must be square");
	U->init(height,width);
	L->init(1,width);
	matrix_eigSym2_float(data, U->data, L->data, width);
}

void Matrix::eig_sym(Matrix* U, Matrix* L, uint nb_eigenvectors) {
	if(!is_square()) throw std::runtime_error("Matrix must be square");
	Matrix _U,_L;
	_U.init(height, width);
	_L.init(1,width);
	matrix_eigSym2_float(data, _U.data, _L.data, width);
	matrix_sortEig_float(_U.data, _L.data, width);
	*U = _U.trim_height(nb_eigenvectors);
	*L = _L.trim_width(nb_eigenvectors);
}

void Matrix::reconstruct(const Matrix& U, const Matrix& L) {
	init(U.width,U.width);
	for(size_t i = 0; i < U.width; i++) {
		for(size_t j = 0; j < U.width; j++) {
			float f = 0;
			for(size_t k = 0; k < L.n; k++) {
				f += L[k] * U(k,i) * U(k,j);
			}
			data[i*width + j] = f;
		}
	}
}

Matrix& Matrix::operator+=(const ScaledMatrix& sm) {
	vector_addm_float(data, sm.f, sm.m.data, n);
	return *this;
}

Matrix& Matrix::operator-=(const ScaledMatrix& sm) {
	vector_addm_float(data, -sm.f, sm.m.data, n);
	return *this;
}

Matrix& Matrix::operator=(const ScaledMatrix& sm) {
	*this = (const Matrix&) sm.m;
	*this *= sm.f;
	return *this;
}


