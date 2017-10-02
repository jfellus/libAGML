#define __algebra_single_header__
/** 
 * @file core.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */


/** 
 * \defgroup algebra [library] algebra: Algebra (C/A+)
 * \ingroup processing
 */

/** 
 * @addtogroup algebra
 * @{
 */


#ifndef __algebra_core_h__
#define __algebra_core_h__

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#include <malloc.h>
#include <math.h>
#include <string.h>

#ifdef __SSE2__
#define ALGEBRA_SSE2
#include <emmintrin.h>
void _mm_print_ps(__m128 x);

#ifdef __SSSE3__
#define ALGEBRA_SSSE3
#include <tmmintrin.h>

#ifdef __SSE4_1__
#define ALGEBRA_SSE4
#include <smmintrin.h>

#ifdef __AVX__
#define ALGEBRA_AVX
#include <immintrin.h>
void _mm256_print_ps(__m256 x);

#endif /* AVX */

#endif /* SSE4_1 */

#endif /* SSSE3 */

#ifdef __AVX__
#define ALGEBRA_FLOAT_GRANULARITY   8
#define ALGEBRA_ALIGN_SIZE          0x20
#else /* Not AVX */
#define ALGEBRA_FLOAT_GRANULARITY   4
#define ALGEBRA_ALIGN_SIZE          0x10
#endif

#define ALGEBRA_IS_ALIGNED(x) ((((size_t)x)%ALGEBRA_ALIGN_SIZE) == 0)

//! Aligned allocation.
static inline void* algebra_allocator(size_t n) { return _mm_malloc(n,ALGEBRA_ALIGN_SIZE); }

#define algebra_alloc_uchar(n)  ((unsigned char*)_mm_malloc((n)*sizeof(unsigned char),ALGEBRA_ALIGN_SIZE))
#define algebra_alloc_short(n)  ((short*)_mm_malloc((n)*sizeof(short),ALGEBRA_ALIGN_SIZE))
#define algebra_alloc_uint(n)  ((unsigned int*)_mm_malloc((n)*sizeof(unsigned int),ALGEBRA_ALIGN_SIZE))
#define algebra_alloc_float(n)  ((float*)_mm_malloc((n)*sizeof(float),ALGEBRA_ALIGN_SIZE))
#define algebra_alloc_double(n) ((double*)_mm_malloc((n)*sizeof(double),ALGEBRA_ALIGN_SIZE))

#define algebra_free(p)   _mm_free(p)

#ifdef __cplusplus
template<class T>
T* algebra_alloc (size_t n) {
    return (T*)_mm_malloc(n*sizeof(T),ALGEBRA_ALIGN_SIZE);
}
template<class T>
class algebra_buffer {
    T* p;
public:
    algebra_buffer(size_t n=0) : p(NULL) {
        if (n > 0) p = algebra_alloc<T>(n);
    }
    algebra_buffer(T* p) : p(p) { }
    ~algebra_buffer() {
        if (p) algebra_free(p);
    }
    T*& get() { return p; }
    const T*& get() const { return p; }
    T* release() { T* q=p; p=NULL; return q; }
    operator T*() { return p; }
    operator const T*() const { return p; }
    T& operator[](size_t i) { return p[i]; }
    const T& operator[](size_t i) const { return p[i]; }
    T* operator+(size_t i) { return p+i; }
    const T* operator+(size_t i) const { return p+i; }
    
};
template<class T>
class algebra_buffer2 {
    T** p;
    size_t n;
public:
    algebra_buffer2(size_t n) : p(NULL),n(n) {
        if (n > 0) {
            p = algebra_alloc<T*>(n);
            for (size_t i=0;i<n;i++) p[i] = NULL;
        }
    }
    algebra_buffer2(T** p,size_t n) : p(p),n(n) { 
    }
    ~algebra_buffer2() {
        if (p) {
            for (size_t i=0;i<n;i++)
                if (p[i]) algebra_free(p[i]);
            algebra_free(p);
        }
    }
    T**& get() { return p;}
    const T* const*& get() const { return p;}
    operator T**() { return p; }
    operator const T* const*() const { return p; }
    T*& operator[](size_t i) { return p[i]; }
    const T*& operator[](size_t i) const { return p[i]; }
    T** operator+(size_t i) { return p+i; }
    const T* const* operator+(size_t i) const { return p+i; }
    
};
#endif

#else /* Not SSE2 */

//! Check if the adress is aligned.
#define ALGEBRA_IS_ALIGNED(x) (1)

//! Aligned allocation.
#define algebra_allocator malloc

//! Aligned allocation of vector.
#define algebra_alloc_uchar(n)  ((unsigned char*)malloc((n)*sizeof(unsigned char)))
//! Aligned allocation of vector.
#define algebra_alloc_short(n)  ((short*)malloc(n*sizeof(short)))
//! Aligned allocation of vector.
#define algebra_alloc_float(n)  ((float*)malloc(n*sizeof(float)))
//! Aligned allocation of vector.
#define algebra_alloc_double(n) ((double*)malloc(n*sizeof(double)))

//! Free aligned buffer.
#define algebra_free(p)   free(p)

#ifdef __cplusplus
template<class T>
T* algebra_alloc (size_t n) {
    return (T*)malloc(n*sizeof(T));
}
#endif

#endif /* SSE2 */

/**
 * \cond INTERNAL
 */

#if defined(__STRICT_ANSI__)  
#define ALGEBRA_INLINE 
#elif defined(__GNUC__)
#define ALGEBRA_INLINE inline
#else
#define ALGEBRA_INLINE 
#endif

/** \endcond */

#ifdef __cplusplus
extern "C" {
#endif

//! Print current arch.
void	algebra_show_arch();

#ifdef __cplusplus
}
#endif

#endif


/** @} */


/** 
 * @file random.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */

#ifndef __algebra_random_h__
#define __algebra_random_h__


#include <stddef.h>
#include <stdint.h>

/** 
 * @addtogroup algebra  
 * @{
 */

extern "C" {
    
/**
 * @name Random generators
 * @{
 */

//! Hold data for random number generation.
typedef struct cmwc_random_generator_ cmwc_random_generator;

//! Create a random number generator using the complementary-multiply-with-carry method.
void    random_new_cmwc_generator (cmwc_random_generator*& rg,uint32_t seed);

//! Delete a random number generator.
void    random_delete_cmwc_generator(cmwc_random_generator*& rg);

//! Return a random uint32 number using the complementary-multiply-with-carry method.
uint32_t random_cmwc_uint32(cmwc_random_generator* rg);

//! Return a random uint64 number using the complementary-multiply-with-carry method.
uint64_t random_cmwc_uint64(cmwc_random_generator* rg);

//! Return a random float number in [0,1[ using the complementary-multiply-with-carry method.
float   random_cmwc_float(cmwc_random_generator* rg);

//! Return a random double number in [0,1[ using the complementary-multiply-with-carry method.
double  random_cmwc_double(cmwc_random_generator* rg);

/** @} */

}

/** @} */

#endif


/** 
 * @file vector_double.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */


#ifndef __algebra_vector_double_h__
#define __algebra_vector_double_h__

#include <stddef.h>

/** 
 * @addtogroup algebra
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Dense vectors, double values
 * @{
 */

/*! Calcule la norme L1. */
double	vector_n1_double (const double* v,size_t n);
/*! Calcule la norme L2. */
double	vector_n2_double (const double* v,size_t n);
/*! Calcule la norme L2 au carré. */
double	vector_n2p2_double (const double* v,size_t n);
/*! Calcule la distance L2 entre deux vectors. */
double	vector_l2_double (const double* v1,const double* v2,size_t n);
/*! Calcule la distance L2 au carré entre deux vectors. */
double	vector_l2p2_double (const double* v1,const double* v2,size_t n);

/*! Calcule le produit scalaire. */
double	vector_ps_double (const double* v1,const double* v2,size_t n);
/*! Calcule le produit scalaire. version bench */
double	vector_ps_double_basic (const double* v1,const double* v2,size_t n);

/*! Calcule la valeur max. */
double	vector_max_double (const double* v,size_t n);

/*! Calcule v1 += v2. */
void	vector_add_double (double* v1,const double* v2,size_t n);
/*! Calcule v1 += lambda*v2. */
void	vector_addm_double (double* v1,double lambda,const double* v2,size_t n);
/*! Calcule v1 -= v2. */
void	vector_sub_double (double* v1,const double* v2,size_t n);
/*! Calcule v1 *= v2. */
void	vector_mul_double (double* v1,double* v2,size_t n);
/*! Calcule v1 /= v2. */
void	vector_div_double (double* v1,double* v2,size_t n);

/*! Calcule v = v1+alpha*v2. */
void	vector_linear_double (double* v,const double* v1,const double alpha,const double* v2,size_t n);
/*! Calcule mean += lambda*v2 et var += lambda*v2^2 (pour calculer moyenne et variance par la suite). */
void	vector_add_stats_double (double* mean,double* var,double lambda,const float* v,size_t n);

/*! Calcule v = r. */
void	vector_scpy_double (double* v,double r,size_t n);
/*! Calcule v += r. */
void	vector_sadd_double (double* v,double r,size_t n);
/*! Calcule v -= r. */
void	vector_ssub_double (double* v,double r,size_t n);
/*! Calcule v *= r. */
void	vector_smul_double (double* v,double r,size_t n);
/*! Calcule v /= r. */
void	vector_sdiv_double (double* v,double r,size_t n);


/*! Calcule p[i] = exp(p[i]) / sum_j(exp(p[j]) de manière prudente, pour éviter les erreurs de précision */
void    vector_exp_proba_double (double* p,size_t n);

/** @} */

#ifdef __cplusplus
}
#endif


/** @} */



#endif
/** 
 * @file vector_float.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */

 
#ifndef __algebra_vector_float_h__
#define __algebra_vector_float_h__

#include <stddef.h>

/** 
 * @addtogroup algebra
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Dense vectors, float values
 * @{
 */
    
    
/*! Matrice aléatoire uniforme */
void    vector_rand_float (float* v,float fMin,float fMax,size_t n);

/*! Calcule le produit scalaire. */
float	vector_ps_float (const float* v1,const float* v2,size_t n);
/*! Calcule le produit scalaire. */
/* Version non optimizée, pour les benchmarks */
float	vector_ps_float_basic (const float* v1,const float* v2,size_t n);
/*! Calcule le produit scalaire par parquet de 32 valeurs.
 * Les deux vecteurs v1 et v2 sont de taille 32*q
 * q doit être non nul
 * Attention : les données doivent être alignées */
float	vector_ps_float_aligned_32 (const float* v1,const float* v2,size_t q);

/*! Cacule la somme des valeurs. */
float	vector_sum_float (const float* v,size_t n);
/*! Cacule la somme des valeurs. */
/* Version non optimizée, pour les benchmarks */
float	vector_sum_float_basic (const float* v,size_t n);

/*! Calcule la norme L1. */
float	vector_n1_float (const float* v,size_t n);
/*! norm L1, version basic pour benchmark */
float	vector_n1_float_basic (const float* v,size_t n);
/*! norme L1, version vectorisée */
float vector_n1_float_aligned_32(const float * v, size_t q);

/*! Calcule la norme L2. */
float	vector_n2_float (const float* v,size_t n);
/*! Calcule la norme L2 au carré. */
float	vector_n2p2_float (const float* v,size_t n);
/*! Calcule la norme L2 au carré.version basic pour bench */
float	vector_n2p2_float_basic (const float* v,size_t n);

/*! Calcule la distance L1 entre deux vectors. */
float	vector_l1_float (float* v1,float* v2,size_t n);
/*! Calcule la distance L2 entre deux vectors. */

float	vector_l2_float (const float* v1,const float* v2,size_t n);
/*! Calcule la distance L2 au carré entre deux vectors. */
float	vector_l2p2_float (const float* v1,const float* v2,size_t n);
/*! Calcule la distance L2 au carré entre deux vectors. version basic pour bench */
float	vector_l2p2_float_basic (const float* v1,const float* v2,size_t n);

/*! Calcule la distance Chi1 entre deux vectors. */
float	vector_chi1_float (float* v1,float* v2,size_t n);
/*! Calcule la distance Chi1 entre deux vectors translatés de t. */
float	vector_chi1t_float (float* v1,float* v2,float* t,size_t n);
/*! Calcule la distance Chi2 entre deux vectors. */
float	vector_chi2_float (float* v1,float* v2,size_t n);
/*! Calcule la similarité cos entre deux vectors. */
float	vector_cos_float (const float* v1,const float* v2,size_t n);
/*! Calcule la distance Linf entre deux vectors. */
float	vector_linf_float (float* v1,float* v2,size_t n);
/*! Calcule la valeur max. */
float	vector_max_float (float* v,size_t n);
/*! Calcule la valeur min. */
float	vector_min_float (float* v,size_t n);
/*! Calcule l'écart type. */
float	vector_std_float (const float* v1,const float* v2,size_t n);
/**
 * Counts the number of time a value is found in a vector
 * 
 * @param v Input vector
 * @param n Input vector size
 * @param x Value to count
 * @param epsilon Tolerance
 * @return Number of time \f$|v_i - x| <= \epsilon\f$ is true.
 */
size_t  vector_count_float (const float* v,size_t n,float x,float epsilon);
/*! Calcule l'indice de la valeur max. */
size_t	vector_argmax_float (float* v,size_t n);
/*! Calcule l'indice de la valeur min. */
size_t	vector_argmin_float (float* v,size_t n);

/*! Calcule v[i] = abs(v[i]). */
void	vector_abs_float (float* v,size_t n);
/*! Calcule v[i] = sgn(v[i])*sqrt(abs(v[i]),p). */
void	vector_sqrt_float (float* v,size_t n);
/*! Calcule v[i] = sgn(v[i])*powf(abs(v[i]),p). */
void	vector_pow_float (float* v,float p,size_t n);
/*! Règle de 3 pour avoir des valeurs entre rMin et rMax. */
void	vector_rescale_float (float* v,float rMin,float rMax,size_t n);
/*! Calcule la norme complexe z[i] = sqrt(x[i]*x[i]+y[i]*y[i]). */
void	vector_cn2_float (float* z,float* x,float* y,size_t n);
/*! Calcule la norme quaternionique. */
void    vector_qn2_float (float* z,float* x,float* yi,float* yj,float* yk,size_t n);

/*! Calcule v1 = lambda*v2. */
void	vector_cpym_float (float* v1,float lambda,const float* v2,size_t n);
/*! Calcule v1 += v2. */
void	vector_add_float (float* v1,const float* v2,size_t n);
/*! Calcule v1 += lambda*v2. */
void	vector_addm_float (float* v1,float lambda,const float* v2,size_t n);
/*! Calcule v1 -= v2. */
void	vector_sub_float (float* v1,const float* v2,size_t n);
/*! Calcule v1 -= v2. Version non optimisée pour les benchmarks */
void	vector_sub_float_basic (float* v1,const float* v2,size_t n);
/*! Calcule v1 *= v2. */
void	vector_mul_float (float* v1,float* v2,size_t n);
/*! Calcule v1 /= v2. */
void	vector_div_float (float* v1,float* v2,size_t n);

/*! Calcule v = v1+alpha*v2. */
void	vector_linear_float (float* v,const float* v1,const float alpha,const float* v2,size_t n);

/*! Calcule v = r. */
void	vector_scpy_float (float* v,float r,size_t n);
/*! Calcule v += r. */
void	vector_sadd_float (float* v,float r,size_t n);
/*! Calcule v -= r. */
void	vector_ssub_float (float* v,float r,size_t n);
/*! Calcule v *= r. */
void	vector_smul_float (float* v,float r,size_t n);
/*! Calcule v /= r. */
void	vector_sdiv_float (float* v,float r,size_t n);


/*! Vérifie que les valeurs du vecteur sont entières. */
int     vector_isfinite_float (const float* v,size_t n);

//! Renvoie l'indice du plus proche voisin, au sens de la distance l2.
size_t  vector_argmin_l2_float (const float* x,const float* C,size_t n,size_t q,const float* normC);

/** @} */


#ifdef __cplusplus
}
#endif


/** @} */



#endif
/** 
 * @file vector_short.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */


#ifndef __algebra_vector_short_h__
#define __algebra_vector_short_h__

#include <stddef.h>

/** 
 * @addtogroup algebra
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * @name Dense vectors, 16-bit signed integer values
 * @{
 */


/*! Calcule le produit scalaire.  */
int	vector_ps_short (const short* v1,const short* v2,size_t n);
/*! Calcule le cosinus.  */
float	vector_cos_short(const short* A,const short* B,size_t n);

/** @} */

#ifdef __cplusplus
}
#endif


/** @} */



#endif
/** 
 * @file vector_uchar.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */


#ifndef __algebra_vector_uchar_h__
#define __algebra_vector_uchar_h__

#include <stddef.h>

/** 
 * @addtogroup algebra
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Dense vectors, 8-bit unsigned integer 
 * @{
 */
    
    
/*! Calcule le produit scalaire. */
unsigned int	vector_ps_uchar (const unsigned char* v1,const unsigned char* v2,size_t n);
/*! Calcule la distance L1 entre deux vectors. */
unsigned int	vector_l1_uchar (const unsigned char* v1,const unsigned char* v2,size_t n);
/*! Calcule la distance L2 au carré entre deux vectors. */
unsigned int	vector_l2p2_uchar (const unsigned char* v1,const unsigned char* v2,size_t n);

/** @} */


#ifdef __cplusplus
}
#endif


/** @} */



#endif
/** 
 * @file matrix_double.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */


#ifndef __algebra_matrix_double_h__
#define __algebra_matrix_double_h__

#include <stddef.h>

/** 
 * @addtogroup algebra
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

 /**
 * @name Dense matrix, double values
 * @{
 */

    
/*! Calcule C += A*B. */
void	matrix_CpAB_double (double* C,double* A,double* B,size_t n,size_t p,size_t m);
/*! Calcule C += A'*B. */
void	matrix_CpAtB_double (double* C,double* A,double* B,size_t n,size_t p,size_t m);
/*! Calcule C += A*B'. */
void	matrix_CpABt_double (double* C,double* A,double* B,size_t n,size_t p,size_t m);
/*! Calcule C += a*a' (pratique pour matrice covariance). */
void	matrix_Cpaat_double (double* C,const double* a,size_t n);
/*! Calcule C += A*B*A'. */
/*! r est un vecteur temporaire de taille m. */
void	matrix_CpABAt_double (double* C,double* A,double* B,size_t n,size_t m,double* r);
/*! Calcule C += A'*B*A. */
/*! r est un vecteur temporaire de taille m. */
void	matrix_CpAtBA_double (double* C,double* A,double* B,size_t n,size_t m,double* r);
/*! Calcule C += A*d*A' (d vecteur diagonale). */
void	matrix_CpAdAt_double (double* C,double* A,double* d,size_t n,size_t m);

/*! Calcule Y = X'. */
void	matrix_t_double (double* Y,double* X,size_t n,size_t m);
/*! Cacule s_{1j} = sum_i X_{ij}. */
void	matrix_sum_double (double* s,double* X,size_t n,size_t m);
/*! Cacule s_{i} = sum_j X_{ij}. */
void	matrix_sumt_double (double* s,double* X,size_t n,size_t m);
/*! Cacule s_{i} = 1/m sum_i X_{ij}. */
void	matrix_meant_double(double* s,const double* X,size_t n,size_t m);
/*! Calcule la distance L2 entre deux matrices. */
double	matrix_l2_double (const double* A,const double* B,size_t n,size_t m);
/*! Calcule la distance L2 au carré entre deux matrices. */
double	matrix_l2p2_double (const double* A,const double* B,size_t n,size_t m);

/*! Décomposition LU. */
int	matrix_lu_double (double* M,size_t* idx,double* d,size_t n);
/*! Backsubstition LU. */
void	matrix_lub_double (double* b,double* M,size_t *idx,size_t n);
/*! Inversion. */
/*! Y: matrix(n,n) inverse.\n
    M: matrix(n,n) à inverser (modifiée).*/
int	matrix_inv_double (double* Y,double* M,size_t n);
/*! Inversion et multiplication Y = inv(M)*B. */
/*! Y: matrix(n,n) inverse.\n
    M: matrix(n,n) à inverser (modifiée).\n
    B: matrix(n,m) à multiplier.*/
int	matrix_invm_double (double* Y,double* M,double* B,size_t n,size_t m);
/*! Décomposition Cholesky. */
size_t	matrix_ll_double (double* L,double* A,size_t n,size_t m);
/*! Décomposition QR. */
size_t	matrix_qr_double (double* A,double* R,size_t n,size_t m);
/*! Décomposition QT. */
void	matrix_qtq_double (double* A,double* d,double* e,size_t n);
/*! Décomposition QT partielle (que le T). */
void	matrix_qtq_t_double (double* A,double* d,double* e,size_t n);
/*! Décomposition complète en valeur et vecteurs propres d'une matrix trigonale. */
void	matrix_eigTrig_double (double* A,double* d,double* e,size_t n);
/*! Calcul des valeurs propres d'une matrix trigonale. */
void	matrix_eigTrig_values_double (double* d,double* e,size_t n);
/*! Décomposition complète en valeur et vecteurs propres d'une matrix symétrique. */
void	matrix_eigSym_double (double* A,double* d,size_t n);
/*! Calcul de la plus grande valeur propre. */
int	matrix_eigMax_double (double* l,double* v,double* M,size_t n);
/*! Centre une matrice symétrique. */
/*! buf: tampon de n doubles (alloué en interne si NULL)
 */
void	matrix_sym_centering_double (double* M,size_t n,double* buf);

/*! Sauvegarde le contenu d'une matrice dans un fichier */
int     matrix_save_double(const char* file_name,const double* M,size_t n,size_t m);

/*! Recherche du vecteur le plus proche dans un dictionnaire. */
/*!
 * sample : vecteur à classifier
 * dict : matrice(n,m) dictionnaire
 * n: taille des vecteurs
 * m: nombre de vecteurs dans le dico.
 */
size_t matrix_argmin_l2_double (const double* sample,const double* dict,size_t n,size_t m);

/** @} */

#ifdef __cplusplus
}
#endif


/** @} */



#endif
/** 
 * @file matrix_float.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */


#ifndef __algebra_matrix_float_h__
#define __algebra_matrix_float_h__

#include <stddef.h>


/** 
 * @addtogroup algebra
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Dense matrix, float values
 * @{
 */

/*! Calcule C += A*B. */
/*! C de taille (n,m)
 *  A de taille (n,p)
 *  B de taille (p,m)
 */
void	matrix_CpAB_float (float* C,const float* A,const float* B,size_t n,size_t p,size_t m);
/*! Calcule C += A'*B. */
void	matrix_CpAtB_float (float* C,const float* A,const float* B,size_t n,size_t p,size_t m);
/*! Calcule C += A*B'. */
/*! C de taille (n,m)
 *  A de taille (p,n)
 *  B de taille (p,m)
 */
void	matrix_CpABt_float (float* C,const float* A,const float* B,size_t n,size_t p,size_t m);
/*! Calcule C += A*A'. */
/*! C de taille (n,n)
 *  A de taille (n,p)
 */
void	matrix_CpAAt_float (float* C,const float* A,size_t n,size_t p);

/*! Calcule C += a*a' (pratique pour matrice covariance). */
void	matrix_Cpaat_float (float* C,const float* a,size_t n);

/*! Calcule C += A*B*A'. */
/*! r est un vecteur temporaire de taille m. */
void	matrix_CpABAt_float (float* C,const float* A,const float* B,size_t n,size_t m,float* r);
/*! Calcule C += A'*B*A. */
/*! r est un vecteur temporaire de taille m. */
void	matrix_CpAtBA_float (float* C,const float* A,const float* B,size_t n,size_t m,float* r);
/*! Calcule C += A*d*A' (d vecteur diagonale). */
void	matrix_CpAdAt_float (float* C,const float* A,const float* d,size_t n,size_t m);

/*! Calcule Y = X'. */
void	matrix_t_float (float* Y,const float* X,size_t n,size_t m);
/*! Calcule X = X' */
void    matrix_selft_float(float* X,size_t n,size_t m);
/*! Calcule Y = X(i:i+n,j:j+m). */
void	matrix_cpy_float (float* Y,size_t iy,size_t jy,const float* X,size_t ix,size_t jx,size_t n,size_t m,size_t nx,size_t ny);
/*! Calcule Y(i,j) = X((i+di)%n,(j+dj)%m). */
void	matrix_rot_float (float* Y,const float* X,size_t di,size_t dj,size_t n,size_t m);
/*! Cacule s_{1j} = sum_i X_{ij}. */
void	matrix_sum_float (float* s,const float* X,size_t n,size_t m);
/*! Cacule s_{i} = sum_j X_{ij}. */
void	matrix_sumt_float (float* s,const float* X,size_t n,size_t m);
/*! Cacule s_{i} = 1/m sum_i X_{ij}. */
void	matrix_meant_float (float* s,const float* X,size_t n,size_t m);
/*! Calcule le produit scalaire (frobenuis) entre deux matrices. */
double	matrix_ps_float (const float* A,const float* B,size_t n,size_t m);
/*! Calcule le produit scalaire (frobenuis) entre xx' et yy'. */
double	matrix_ps_xxt_yyt_float (const float* x,const float* y,size_t n);
/*! Calcule le produit scalaire (frobenuis) entre XX' et YY'. */
double	matrix_ps_XXt_YYt_float (const float* X,size_t p,const float* Y,size_t q,size_t n);
/*! Calcule la norme L2. */
double	matrix_n2_float (const float* v,size_t n,size_t m);
/*! Calcule la distance L2 entre deux matrices. */
double	matrix_l2_float (const float* A,const float* B,size_t n,size_t m);
/*! Calcule la distance L2 au carré entre deux matrices. */
double	matrix_l2p2_float (const float* A,const float* B,size_t n,size_t m);

/* Calculs centrés, avec H = Identité - 1/n */

/*! Calcule la norme centré tr(HKHK). */
double  matrix_cn2_float (const float* K,size_t n);
/*! Calcule le produit scalaire centré tr(HKHL). */
/*! K et L sont deux matrices de taille n x n */
double  matrix_cps_float(const float* K,const float* L,size_t n);
/*! Calcule le produit scalaire centré entre xx' et yy'. */
/*! x et y sont des vecteurs de taille n */
double	matrix_cps_xxt_yyt_float (const float* x,const float* y,size_t n);
/*! Calcule le produit scalaire centré entre XX' et yy'. */
/*! X est une matrice de p vecteurs de taille n
 *  y est un vecteur de taille n */
double	matrix_cps_XXt_yyt_float (const float* X,size_t p,const float* y,size_t n);
/*! Calcule le produit scalaire centré entre XX' et YY'. */
/*! X est une matrice de p vecteurs de taille n
 *  Y est une matrice de q vecteurs de taille n */
double	matrix_cps_XXt_YYt_float (const float* X,size_t p,const float* Y,size_t q,size_t n);
/**
 * Calcul la matrice diagonal: diag(HXX'Hyy')
 * @param X est une matrice de p vecteurs de taille n
 * @param p est la taille des vecteurs de X
 * @param y est un vecteur de taille n
 * @param n 
 * @param res est le vecteur resultat de taille n
 */
void    matrix_diag_XXt_yyt_float (const float* X,size_t p,const float* y,size_t n,float *res);

/*! Calcule le produit scalaire centré entre X'X et Y'Y. */
/*! X est une matrice de n vecteurs de taille p
 *  Y est une matrice de n vecteurs de taille q */
double	matrix_cps_XtX_YtY_float (const float* X,size_t p,const float* Y,size_t q,size_t n);

/*! Décomposition LU. */
int     matrix_lu_float (float* M,size_t* idx,float* d,size_t n);
/*! Backsubstition LU. */
void	matrix_lub_float (float* b,float* M,size_t *idx,size_t n);
/*! Inversion. */
/*! Y: matrix(n,n) inverse.\n
    M: matrix(n,n) à inverser (modifiée).*/
int     matrix_inv_float (float* Y,float* M,size_t n);
/*! Inversion. (Using Eigen library) */
/*! inv: matrix(n,n) inverse.\n
    A: matrix(n,n) à inverser.*/
void matrix_inv2_float(float* inv, float* A, size_t n);
/*! Inversion et multiplication Y = inv(M)*B. */
/*! Y: matrix(n,n) inverse.\n
    M: matrix(n,n) à inverser (modifiée).\n
    B: matrix(n,m) à multiplier.*/
int     matrix_invm_float (float* Y,float* M,float* B,size_t n,size_t m);
/*! Inverse de la matrice de covariance C_{ij} = X_{i}^t X^{j} (=> Moindre carrés/LLE). */
/*! invC : matrix(m,m) inverse
 *  X : matrix(n,m) de m vecteurs de dimension n (modifiée apres appel)
 *  Renvoie zero si n<m, sinon le rang de la matrice de covariance */
size_t	matrix_inv_cov_float (float* invC,float* X,size_t n,size_t m);
/*! Approximation LLE. */
/*! y : vecteur(m) coefficients
 *  x : vecteur(m) à approximer
 *  X : matrix(n,m) base de m vecteurs de dimension n
 *  invC : matrix(m,m) inverse des covariances de X */
void	matrix_lle_float(float* y,const float* x,const float* X,const float* invC,size_t n,size_t m);
/*! Décomposition Cholesky. */
size_t	matrix_ll_float (float* L,float* A,size_t n,size_t m);
/*! Inversion d'une matrice triangulaire inférieure. */
/*! Y: matrix(n,n) inverse.\n
    L: matrix(n,n) à inverser.*/
void	matrix_inv_r_float (float* Y,float* L,size_t n);
/*! Décomposition QR. */
size_t	matrix_qr_float (float* A,float* R,size_t n,size_t m);
/*! Décomposition QR 'thin/economique' (si m>n, ne calcule que les n premiers facteurs de Q) */
void    matrix_qr_thin_float(float* _A, float* _Q, float *_R, size_t n, size_t m);
/*! Décomposition QT. */
void	matrix_qtq_float (float* A,float* d,float* e,size_t n);
/*! Décomposition QT partielle (que le T). */
void	matrix_qtq_t_float (float* A,float* d,float* e,size_t n);
/*! Décomposition complète en valeur et vecteurs propres d'une matrix trigonale. */
void	matrix_eigTrig_float (float* A,float* d,float* e,size_t n);
/*! Calcul des valeurs propres d'une matrix trigonale. */
void	matrix_eigTrig_values_float (float* d,float* e,size_t n);
/*! Décomposition complète en valeur et vecteurs propres d'une matrix symétrique. */
void	matrix_eigSym_float (float* A, float* d,size_t n);
/*! Décomposition complète en valeur et vecteurs propres d'une matrix symétrique. */
void	matrix_eigSym2_float (const float* M,float* A, float* d,size_t n);
/*! Trie dans l'ordre décroissant les valeurs propres. */
void	matrix_sortEig_float (float* A,float* d,size_t n);
/*! Calcul de la plus grande valeur propre. */
int     matrix_eigMax_float (float* l,float* v,float* M,size_t n);
/*! Centre une matrice symétrique. */
/*! buf: tampon de n floats (alloué en interne si NULL)
 */
void	matrix_sym_centering_float (float* M,size_t n,float* buf);


/*! Recherche du vecteur le plus proche dans un dictionnaire. */
/*!
 * sample : vecteur à classifier
 * dict : matrice(n,m) dictionnaire
 * n: taille des vecteurs
 * m: nombre de vecteurs dans le dico.
 */
size_t  matrix_argmin_l2_float (const float* sample,const float* dict,size_t n,size_t m);

/** @} */

#ifdef __cplusplus
}
#endif


/** @} */



#endif
/** 
 * @file sparse_vector_float.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */


#ifndef __algebra_sparse_vector_float_h__
#define __algebra_sparse_vector_float_h__

#ifndef __algebra_single_header__
#include "core.h"
#include "vector_float.h"
#endif

/** 
 * @addtogroup algebra
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif
    
/**
 * @name Sparse vectors, float values
 * @{
 */

/**
 * Create a sparse vector (float and uint32_t buffers couple) from a dense vector.
 * Allocation are performed using algebra_alloc_xxxx
 * 
 * @param[out] x Allocated value buffer. Must be NULL.
 * @param[out] xi Allocated index buffer. Must be NULL.
 * @param[out] xn Size of buffers.
 * @param[in] y Input dense vector.
 * @param[in] yn Size of input dense vector.
 */
void  sparse_vector_alloc_float (float** x,uint32_t** xi,uint32_t* xn,const float* y,uint32_t yn);

/**
 * Delete a sparse vector (float and uint32_t buffers couple).
 * Deletions are performed using algebra_free()
 * 
 * @param[inout] x Deleted value buffer. Pointed value set to NULL after call. Ignored if pointed value is NULL. 
 * @param[inout] xi Deleted index buffer. Pointed value set to NULL after call. Ignored if pointed value is NULL.
 * @param[in] xn Pointed value set to 0 after call.
 */
void  sparse_vector_free_float (float** x,uint32_t** xi,uint32_t* xn);

/**
 * Fill a dense vector using values from a sparse vector.
 * Note that this function does not perform any allocation, output dense vector
 * must have the right size.
 * 
 * @param[out] y Output dense vector.
 * @param[in] yn Size of output dense vector.
 * @param[in] x Input value buffer. 
 * @param[in] xi Input index buffer.
 * @param[in] xn Size of value and index buffers.
 * @param[in] safe If true, will check that index in xi are not higher or equal to yn.
 * Throw an exception if an invalid value is found.
 * Otherwise, in case of overflow, it will crash.
 */
void  sparse_vector_expand_float (float* y,uint32_t yn,const float* x,const uint32_t* xi,uint32_t xn,bool safe=false);

/**
 * Shows a string representation of a sparse vector
 * 
 * @param[out] stream Standard stream
 * @param[in] x Input value buffer. 
 * @param[in] xi Input index buffer.
 * @param[in] xn Size of value and index buffers.
 */
void  sparse_vector_fprint (FILE* stream,const float* x,const uint32_t* xi,uint32_t xn);

/**
 * \f$ \forall i\ x_i \leftarrow |x_i|^p \f$ 
 * 
 * Same as vector_pow_float()
 * 
 * @param[in] x Input value buffer. 
 * @param[in] xi Input index buffer. Ignored.
 * @param[in] xn Size of value and index buffers.
 * @param[in] p Power
 */
inline void  sparse_vector_pow_float (float* x,const uint32_t* xi,uint32_t xn,float p) { vector_pow_float(x,p,xn); }

/**
 * \f$ \forall i\ x_i \leftarrow w x_i \f$ 
 * 
 * Same as vector_smul_float()
 * 
 * @param[in] x Input value buffer. 
 * @param[in] xi Input index buffer. Ignored.
 * @param[in] xn Size of value and index buffers.
 * @param[in] w Value
 */
inline void  sparse_vector_smul_float (float* x,const uint32_t* xi,uint32_t xn,float w) { vector_smul_float(x,w,xn); }

/**
 * Returns \f$ \sqrt{\sum_i x_i^2} \f$ 
 * 
 * Same as vector_n2_float()
 * 
 * @param[in] x Input value buffer. 
 * @param[in] xi Input index buffer. Ignored.
 * @param[in] xn Size of value and index buffers.
 * @return l2 norm
 */
inline float  sparse_vector_n2_float (const float* x,const uint32_t* xi,uint32_t xn) { return vector_n2_float(x,xn); }

/**
 * Returns \f$ \sum_i x_i y_i \f$ (a.k.a. the dot product)
 * 
 * @param[in] x Input value buffer. 
 * @param[in] xi Input index buffer.
 * @param[in] xn Size of value and index buffers.
 * @param[in] y Input value buffer. 
 * @param[in] yi Input index buffer.
 * @param[in] yn Size of value and index buffers.
 * @return dot product
 */
float sparse_vector_dp_float (const float* x,const uint32_t* xi,uint32_t xn,const float* y,const uint32_t* yi,uint32_t yn);


/**
 * Returns \f$ \sum_i x_i y_i \f$ (a.k.a. the dot product)
 * 
 * @param[in] x Input value buffer. 
 * @param[in] xi Input index buffer.
 * @param[in] xn Size of value and index buffers.
 * @param[in] y Input dense vector.
 * @param[in] yn Size of dense vector.
 * @return dot product
 */
float sparse_vector_dp_dense_float (const float* x,const uint32_t* xi,uint32_t xn,const float* y,uint32_t yn);

/** @} */
    

#ifdef __cplusplus
}
#endif


/** @} */



#endif
