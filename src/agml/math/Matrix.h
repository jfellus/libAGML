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

#ifndef AGML_MATRIX_H_
#define AGML_MATRIX_H_

#include <stdexcept>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <exception>
#include "math.h"
#include <util/utils.h>
#include <string>
#include <algorithm>

/* Special matrices */
class ScaledMatrix;
class WidthTrimedMatrix;
class HeightTrimedMatrix;
class Matrix;

class WidthTrimedMatrix {
public:
	const Matrix& m;
	size_t w;
	WidthTrimedMatrix(const Matrix& m, size_t w) : m(m),w(w) {}
	operator Matrix();
};
class HeightTrimedMatrix {
public:
	const Matrix& m;
	size_t h;
	HeightTrimedMatrix(const Matrix& m, size_t h) : m(m),h(h) {}
	operator Matrix();
};


class Matrix {
public:
	size_t height,width;
	size_t n;
	float* data;
	bool bDeleteData;

public:
	Matrix() { data = 0; height = width = n = 0; bDeleteData = false; }
	virtual ~Matrix() {
		if(data && bDeleteData)  delete[] data;
	}

	inline Matrix(const char* s) {data = 0; *this = (std::string(s));}
	Matrix(const std::string& s) {data = 0; *this = s;}

	Matrix(float* data, size_t height, size_t width = 1) {
		this->data = data;
		this->height = height;
		this->width = width;
		n = height*width;
		bDeleteData = false;
	}

	Matrix(Matrix& m) {
		data = 0; *this = m;
	}

	Matrix(const Matrix& m) {
		height = m.height;
		width = m.width;
		n = m.n;
		data = m.data;
		bDeleteData = m.bDeleteData;
		((Matrix&)m).bDeleteData = false;
	}

	Matrix(size_t height, size_t width) {
		data = 0;
		init(height,width);
	}

	void init(size_t height, size_t width = 1) {
		if(data) clear();
		this->height = height;
		this->width = width;
		n = height*width;
		data = new float[n];
		bDeleteData = true;
	}

	void init(const Matrix& m) {init(m.height, m.width);}

	bool read(const std::string& file);
	bool write(const std::string& file);

	inline Matrix row(int i) { return Matrix(&data[i*width], 1, width); }
	inline Matrix row(int i) const { return Matrix(&data[i*width], 1, width); }


	inline float& operator[](int i) { return data[i]; }
	inline float operator[](int i) const { return data[i]; }
	inline float& operator()(int i, int j) { return data[i*width + j]; }
	inline float operator()(int i, int j) const { return data[i*width + j]; }
	inline operator float*() {return data;}
	inline operator bool() {return data!=0;}


	bool is_square() {return width==height;}

	inline Matrix& operator=(float v) {	for(size_t i=0; i<n; i++) data[i] = v; return *this; }
	Matrix& operator=(const std::string& s);

	inline Matrix& operator=(const Matrix& mat) {
		if(!data) init(mat);
		if(mat.width != width || mat.height != height) throw std::runtime_error("Matrix dimensions must agree");
		memcpy(data, mat.data, n*sizeof(float));
		return *this;
	}

	inline Matrix& operator=(Matrix& mat) {
		if(!data) {
			this->height = mat.height;
			this->width = mat.width;
			this->n = mat.n;
			this->data = mat.data;
			this->bDeleteData = mat.bDeleteData;
			mat.bDeleteData = false;
		} else {
			if(mat.width != width || mat.height != height) throw std::runtime_error("Matrix dimensions must agree");
			memcpy(data, mat.data, n);
		}
		return *this;
	}

	Matrix& operator*=(float f);
	Matrix& operator/=(float f);

	Matrix& operator+=(const Matrix& m);
	Matrix& operator-=(const Matrix& m);


	inline Matrix operator-(const Matrix& m) {Matrix res ((const Matrix&) *this); res-=m; return res; }
	inline Matrix operator+(const Matrix& m) {Matrix res ((const Matrix&) *this); res+=m; return res; }

	inline Matrix& operator+=(float f) {for(size_t i = 0; i<n; i++) data[i] += f; return *this;}

	inline Matrix& randf() { for(size_t i=0; i<n; i++) data[i] = ::randf(); return *this;}
	inline Matrix& randf(float min, float max) { for(size_t i=0; i<n; i++) data[i] = ::randf(min,max); return *this;}


	float l2p2(const Matrix& m) const;
	float l2(const Matrix& m) const;
	float n2p2() const;
	float n2() const;

	Matrix toDiag() const {
		Matrix res(n,n);
		res = 0;
		for(size_t i=0; i<n; i++) res(i,i) = data[i];
		return res;
	}

	Matrix transpose() const {
		Matrix res(width,height);
		for(size_t i=0; i<height; i++) {
			for(size_t j=0; j<width; j++) {
				res.data[j*height + i] = data[i*width + j];
			}
		}
		return res;
	}

	Matrix diag() {
		if(!is_square()) throw std::runtime_error("Matrix must be square");
		Matrix res(1,width);
		for(size_t i=0; i<height; i++) res[i] = data[i*width+i];
		return res;
	}

	bool check_NaN() const {
		bool b = false;
		for(size_t i=0; i<n; i++) {
			if(isnan(data[i])) { ERROR("NaN at " << i); sleep(1); b = true;}
		}
		return b;
	}

	Matrix part(size_t iPart, size_t nParts) {
		size_t h = height/nParts;
		if(h==0) return iPart>=height ? Matrix() : Matrix(&data[iPart*width], 1, width);
		size_t i = iPart*h;
		if(iPart==nParts-1) h = height-i;
		return Matrix(&data[i*width], h, width);
	}

	WidthTrimedMatrix trim_width(uint w);
	HeightTrimedMatrix trim_height(uint h);


	inline std::string sdims() const {return TOSTRING(height << "x" << width); }


	Matrix col_sums();
	Matrix row_sums();


	Matrix gram();
	Matrix correlation();
	void eig_sym(Matrix* U, Matrix* L);
	void eig_sym(Matrix* U, Matrix* L, uint nb_eigenvectors);
	void reconstruct(const Matrix& U, const Matrix& L);

	Matrix& scale(float f) {return (*this)*=f;}
	Matrix& scale(const Matrix& v);

	Matrix sqrt() {
		Matrix m(height, width);
		for(size_t i=0; i<n; i++) m.data[i] = data[i] >= 0 ? sqrtf(data[i]) : -sqrtf(-data[i]);
		return m;
	}

	Matrix pow(float f) {
		if(f==1) return (const Matrix&)*this;
		else if(f==0.5) return sqrt();
		else if(f==-1) {
			Matrix m(height, width);
			for(size_t i=0; i<n; i++) m.data[i] = 1.0/data[i];
			return m;
		} else if(f==-0.5) {
			Matrix m(height, width);
			for(size_t i=0; i<n; i++) m.data[i] = data[i] >= 0 ? 1.0f/sqrtf(data[i]) : -1.0f/sqrtf(-data[i]);
			return m;
		}
		Matrix m(height, width);
		for(size_t i=0; i<n; i++) m.data[i] = powf(data[i],f);
		return m;
	}

	Matrix operator*(const Matrix& m);

	Matrix inv();

	Matrix& clear() { if(data && bDeleteData) delete[] data; data = 0; width = height = n = 0; return *this;}

	void qr(Matrix* Q, Matrix* R);

	void dump(bool bExact = false);

	float dot(const Matrix& m) const;


	Matrix& operator+=(const ScaledMatrix& sm);
	Matrix& operator-=(const ScaledMatrix& sm);
	Matrix& operator=(const ScaledMatrix& sm);

	Matrix& operator+=(const WidthTrimedMatrix& m);
	Matrix& operator-=(const WidthTrimedMatrix& m);
	Matrix& operator=(const WidthTrimedMatrix& m);
	Matrix& operator+=(const HeightTrimedMatrix& m);
	Matrix& operator-=(const HeightTrimedMatrix& m);
	Matrix& operator=(const HeightTrimedMatrix& m);
};



class ScaledMatrix {
public:
	const Matrix& m;
	float f;
	ScaledMatrix(const Matrix& m, float f) : m(m),f(f) {}

	operator Matrix() {
		Matrix res = m;
		res *= f;
		return res;
	}
};
inline ScaledMatrix operator*(const Matrix& m, float f) {return ScaledMatrix(m,f);}
inline ScaledMatrix operator*(float f, const Matrix& m) {return ScaledMatrix(m,f);}



#endif /* AGML_MATRIX_H_ */
