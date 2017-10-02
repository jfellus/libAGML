#define __veccodec_single_header__
/** 
 * @file veccodec.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */

/** 
 * \defgroup veccodec [library] veccodec: Vector codecs (C/A+)
 * \ingroup codec
 */

/** 
 * @addtogroup veccodec
 * @{
 */

#ifndef __veccodec_h__
#define __veccodec_h__

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @name File formats
 * @{
 */

//! Invalid or unsupported format.
#define VECCODEC_FORMAT_INVALID         -1
//! Automatic detection of format.
#define VECCODEC_FORMAT_AUTODETECT      0

//! Raw format, unsigned int 8-bit      
#define VECCODEC_FORMAT_RAW_UINT8       0x000108
//! Raw format, signed int 8-bit      
#define VECCODEC_FORMAT_RAW_INT8        0x000208

//! Raw format, unsigned int 16-bit      
#define VECCODEC_FORMAT_RAW_UINT16      0x000110
//! Raw format, signed int 16-bit      
#define VECCODEC_FORMAT_RAW_INT16       0x000210

//! Raw format, unsigned int 32-bit      
#define VECCODEC_FORMAT_RAW_UINT32      0x000120
//! Raw format, signed int 32-bit      
#define VECCODEC_FORMAT_RAW_INT32       0x000220
//! Raw format, float
#define VECCODEC_FORMAT_RAW_FLOAT       0x000320

//! Raw format, unsigned int 64-bit      
#define VECCODEC_FORMAT_RAW_UINT64      0x000140
//! Raw format, signed int 64-bit      
#define VECCODEC_FORMAT_RAW_INT64       0x000240
//! Raw format, double
#define VECCODEC_FORMAT_RAW_DOUBLE      0x000340

//! Float vector lists format.
#define VECCODEC_FORMAT_HVEC8           (0x010000 + VECCODEC_FORMAT_RAW_UINT8)
//! Float vector lists format.
#define VECCODEC_FORMAT_FVECS           (0x010000 + VECCODEC_FORMAT_RAW_FLOAT)
//! Double vector lists format.
#define VECCODEC_FORMAT_DVECS           (0x010000 + VECCODEC_FORMAT_RAW_DOUBLE)

//! Siftgeo format.
#define VECCODEC_FORMAT_SIFTGEO         (0x020000 + VECCODEC_FORMAT_RAW_UINT8)

//! CSV format.
#define VECCODEC_FORMAT_CSV             (0x030000 + VECCODEC_FORMAT_RAW_DOUBLE)

//! M format.
#define VECCODEC_FORMAT_M               (0x040000 + VECCODEC_FORMAT_RAW_DOUBLE)

//! MATLAB version 4 format.
#define VECCODEC_FORMAT_MATV4           0x050000
//! MATLAB version 4 format, unsigned int 8-bit.
#define VECCODEC_FORMAT_MATV4_UINT8     (VECCODEC_FORMAT_MATV4 + VECCODEC_FORMAT_RAW_UINT8)
//! MATLAB version 4 format, unsigned int 16-bit.
#define VECCODEC_FORMAT_MATV4_UINT16    (VECCODEC_FORMAT_MATV4 + VECCODEC_FORMAT_RAW_UINT16)
//! MATLAB version 4 format, int 16-bit.
#define VECCODEC_FORMAT_MATV4_INT16     (VECCODEC_FORMAT_MATV4 + VECCODEC_FORMAT_RAW_INT16)
//! MATLAB version 4 format, int 32-bit.
#define VECCODEC_FORMAT_MATV4_INT32     (VECCODEC_FORMAT_MATV4 + VECCODEC_FORMAT_RAW_INT32)
//! MATLAB version 4 format, float.
#define VECCODEC_FORMAT_MATV4_FLOAT     (VECCODEC_FORMAT_MATV4 + VECCODEC_FORMAT_RAW_FLOAT)
//! MATLAB version 4 format, double.
#define VECCODEC_FORMAT_MATV4_DOUBLE    (VECCODEC_FORMAT_MATV4 + VECCODEC_FORMAT_RAW_DOUBLE)
    
    
/** @} */
    
//! Default chunk size for file I/O
#define VECCODEC_DEFAULT_FILE_CHUNK_SIZE 1024*1024
    

/** 
 * @name Generic
 * @{
 */

/*!
 * Detect file format.
 * If file_name is not NULL, this function first use extension to guess format.
 * If not NULL, header contains the first bytes of a file (at least 4 bytes). It may be used in cases where file extension is not provised or not enough for detection.
 * 
 * @param[in] file_name Input file. Ignored if NULL.
 * @param[in] header Header. Ignored if NULL.
 * @param[in] header_size Header size.
 * @return VECCODEC_FORMAT_xxx value
 * 
 */
int     veccodec_detect_format (const char* file_name,const uint8_t* header=NULL,size_t header_size=0);

    
/*!
 * Load a vector file, output in a float buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Load the file in a single row.
 * Should not be used for loading files twice larger than RAM.
 * 
 * @param[out] vectors Upon success, points to a newly allocated buffer that contains the vectors. Must be NULL.
 * @param[out] vector_dim Dimension of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[out] vector_count Number of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[in] file_name File to read.
 * @param[in] format File format (default auto detect)
 * @param[in] allocator Function called for allocation (default std malloc).
 * 
 */
void    veccodec_load_float (float*& vectors,size_t& vector_dim,size_t& vector_count,const char* file_name,int format=VECCODEC_FORMAT_AUTODETECT,void*(*allocator)(size_t)=malloc);
 
/*!
 * Save a vector file, intput from a float buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Save the file in a single row.
 * Should not be used for saving files twice larger than RAM.
 * 
 * @param[in] vectors Vectors
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @param[in] file_name file to write 
 * @param[in] format File format (default auto detect)
 * 
 */
void    veccodec_save_float (const float* vectors,const size_t vector_dim,const size_t vector_count,const char* file_name,int format=VECCODEC_FORMAT_AUTODETECT);

/*!
 * Load a vector file, output in a double buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Load the file in a single row.
 * Should not be used for loading files twice larger than RAM.
 * 
 * @param[out] vectors Upon success, points to a newly allocated buffer that contains the vectors. Must be NULL.
 * @param[out] vector_dim Dimension of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[out] vector_count Number of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[in] file_name File to read.
 * @param[in] format File format (default auto detect)
 * @param[in] allocator Function called for allocation (default std malloc).
 * 
 */
void    veccodec_load_double (double*& vectors,size_t& vector_dim,size_t& vector_count,const char* file_name,int format=VECCODEC_FORMAT_AUTODETECT,void*(*allocator)(size_t)=malloc);

/*!
 * Save a vector file, intput from a double buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Save the file in a single row.
 * Should not be used for saving files twice larger than RAM.
 * 
 * @param[in] vectors Vectors
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @param[in] file_name file to write 
 * @param[in] format File format (default auto detect)
 * 
 */
void    veccodec_save_double (const double* vectors,const size_t vector_dim,const size_t vector_count,const char* file_name,int format=VECCODEC_FORMAT_AUTODETECT);


/** @} */

#ifdef __cplusplus
}
#endif

#endif

/** @} */

/** 
 * @file csv_codec.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */

/** 
 * @addtogroup veccodec
 * @{
 */

#ifndef __csv_codec_h__
#define __csv_codec_h__

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <fstream>

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @name CSV double
 * @{
 */

/*!
 * Load a csv file, output in a double buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Load the file in a single row.
 * Should not be used for loading files twice larger than RAM.
 * 
 * @param[out] vectors Upon success, points to a newly allocated buffer that contains the vectors. Must be NULL.
 * @param[out] vector_dim Dimension of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[out] vector_count Number of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[in] file_name File to read.
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @param[in] allocator Function called for allocation (default std malloc).
 * 
 */
void    veccodec_load_csv_double (double*& vectors,size_t& vector_dim,size_t& vector_count,const char* file_name,char vsep=',',char lsep='\n',void*(*allocator)(size_t)=malloc);

/*!
 * Decode vectors for csv buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * @param[out] vectors If not NULL, double buffer where vectors are saved. If NULL, this function can be used to compte the dimension and count of vectors.
 * @param[in] vector_dim Dimension of vectors. If vectors is not NULL, it will be checked. Otherwise, it will contain the dimension of vectors.
 * @param[in] vector_count Number of vectors. If vectors is not NULL, it will be checked. Otherwise, it will contain the number of vectors.
 * @param[in] buffer Input buffer. Must not be NULL.
 * @param[in] buffer_size Buffer size.
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @return The size of vectors buffer.
 * 
 */
size_t  veccodec_decode_csv_double (double* vectors,size_t& vector_dim,size_t& vector_count,const uint8_t* buffer,size_t buffer_size,char vsep=',',char lsep='\n');

/*!
 * Save a vtext file, intput from a double buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Save the file in a single row.
 * Should not be used for saving files twice larger than RAM.
 * 
 * @param[in] vectors Vectors
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @param[in] format Encoding format (default "%f")
 * @param[in] file_name file to write 
 * 
 */
void    veccodec_save_csv_double (const double* vectors,const size_t vector_dim,const size_t vector_count,const char* file_name,char vsep=',',char lsep='\n',const char* format="%f");

/*!
 * Encode vectors into a buffer with vtext format.
 * This version assumes that all vectors have the same dimension.
 *
 * @param[out] buffer If not NULL, vectors are encoded in this buffer. Otherwise, this function can be used for computing the size of encoded vectors.
 * @param[in] buffer_size Size of the buffer. The function will throw an exception if the buffer is not large enough. Ignored when buffer is NULL.
 * @param[in] vectors Vectors   
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @param[in] format Encoding format (default "%f")
 * @return The size of encoded vectors.
 * 
 */
size_t    veccodec_encode_csv_double (uint8_t* buffer,size_t buffer_size,const double* vectors,size_t vector_dim,size_t vector_count,char vsep=',',char lsep='\n',const char* format="%f");

/** 
 * @}
 */


/** 
 * @name CSV float
 * @{
 */

/*!
 * Load a csv file, output in a float buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Load the file in a single row.
 * Should not be used for loading files twice larger than RAM.
 * 
 * @param[out] vectors Upon success, points to a newly allocated buffer that contains the vectors. Must be NULL.
 * @param[out] vector_dim Dimension of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[out] vector_count Number of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[in] file_name File to read.
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @param[in] allocator Function called for allocation (default std malloc).
 * 
 */
void    veccodec_load_csv_float (float*& vectors,size_t& vector_dim,size_t& vector_count,const char* file_name,char vsep=',',char lsep='\n',void*(*allocator)(size_t)=malloc);

/*!
 * Decode vectors for csv buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * @param[out] vectors If not NULL, float buffer where vectors are saved. If NULL, this function can be used to compte the dimension and count of vectors.
 * @param[in] vector_dim Dimension of vectors. If vectors is not NULL, it will be checked. Otherwise, it will contain the dimension of vectors.
 * @param[in] vector_count Number of vectors. If vectors is not NULL, it will be checked. Otherwise, it will contain the number of vectors.
 * @param[in] buffer Input buffer. Must not be NULL.
 * @param[in] buffer_size Buffer size.
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @return The size of vectors buffer.
 * 
 */
size_t  veccodec_decode_csv_float (float* vectors,size_t& vector_dim,size_t& vector_count,const uint8_t* buffer,size_t buffer_size,char vsep=',',char lsep='\n');

/*!
 * Save a vtext file, intput from a float buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Save the file in a single row.
 * Should not be used for saving files twice larger than RAM.
 * 
 * @param[in] vectors Vectors
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @param[in] format Encoding format (default "%f")
 * @param[in] file_name file to write 
 * 
 */
void    veccodec_save_csv_float (const float* vectors,const size_t vector_dim,const size_t vector_count,const char* file_name,char vsep=',',char lsep='\n',const char* format="%f");

/*!
 * Encode vectors into a buffer with vtext format.
 * This version assumes that all vectors have the same dimension.
 *
 * @param[out] buffer If not NULL, vectors are encoded in this buffer. Otherwise, this function can be used for computing the size of encoded vectors.
 * @param[in] buffer_size Size of the buffer. The function will throw an exception if the buffer is not large enough. Ignored when buffer is NULL.
 * @param[in] vectors Vectors   
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @param[in] format Encoding format (default "%f")
 * @return The size of encoded vectors.
 * 
 */
size_t    veccodec_encode_csv_float (uint8_t* buffer,size_t buffer_size,const float* vectors,size_t vector_dim,size_t vector_count,char vsep=',',char lsep='\n',const char* format="%f");

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/** @} */

/** 
 * @file fvecs_codec.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */

/** 
 * @addtogroup veccodec
 * @{
 */

#ifndef __fvecs_codec_h__
#define __fvecs_codec_h__

#ifndef __veccodec_single_header__
#include "veccodec.h"
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <fstream>

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @name Fvecs
 * @{
 */


/*!
 * Load a fvecs file, output in a float buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Load the file in a single row.
 * Should not be used for loading files twice larger than RAM.
 * 
 * @param[out] vectors Upon success, points to a newly allocated buffer that contains the vectors. Must be NULL.
 * @param[out] vector_dim Dimension of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[out] vector_count Number of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[in] file_name File to read.
 * @param[in] allocator Function called for allocation (default std malloc).
 * 
 */
void    veccodec_load_fvecs (float*& vectors,size_t& vector_dim,size_t& vector_count,const char* file_name,void*(*allocator)(size_t)=malloc);

/*!
 * Load a fvecs file header, and compute dimension and feature count.
 * This version assumes that all vectors have the same dimension.
 *
 * @param[in,out] in Input stream.
 * @param[out] vector_dim Dimension of vectors. Must be 0.
 * @param[out] vector_count Number of vectors. Must be 0.
 * @return The file size.
 * 
 */
size_t    veccodec_read_fvecs_header (std::istream& in,size_t& vector_dim,size_t& vector_count);

/*!
 * Decode vectors for fvecs buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * @param[out] vectors If not NULL, float buffer where vectors are saved. 
 * @param[in] vector_dim Dimension of vectors. All vectors are expected to have this dimension.
 * @param[in] vector_count Number of vectors to decode.
 * @param[in] buffer Input buffer. If NULL, this function can be used to compute the size of this buffer.
 * re vectors are encoded.
 * @param[in] buffer_size Buffer size.
 * @return The size of float buffer.
 * 
 */
size_t  veccodec_decode_fvecs (float* vectors,size_t vector_dim,size_t vector_count,const uint8_t* buffer,size_t buffer_size);

/*!
 * Save a fvecs file, intput from a float buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Save the file in a single row.
 * Should not be used for saving files twice larger than RAM.
 * 
 * @param[in] vectors Vectors
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @param[in] file_name file to write 
 * @param[in] chunk_size Data is written to disk by chunks, to minimize I/O. This parameter selects the size of these chunks.
 * 
 */
void    veccodec_save_fvecs (const float* vectors,const size_t vector_dim,const size_t vector_count,const char* file_name,size_t chunk_size=VECCODEC_DEFAULT_FILE_CHUNK_SIZE);

/*!
 * Encode vectors into a buffer with fvecs format.
 * This version assumes that all vectors have the same dimension.
 *
 * @param[out] buffer If not NULL, vectors are encoded in this buffer. Otherwise, this function can be used for computing the size of encoded vectors.
 * @param[in] buffer_size Size of the buffer. The function will throw an exception if the buffer is not large enough. Ignored when buffer is NULL.
 * @param[in] vectors Vectors   
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @return The size of encoded vectors.
 * 
 */
size_t    veccodec_encode_fvecs (uint8_t* buffer,size_t buffer_size,const float* vectors,size_t vector_dim,size_t vector_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/** @} */

/** 
 * @file dvecs_codec.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */

/** 
 * @addtogroup veccodec
 * @{
 */

#ifndef __dvecs_codec_h__
#define __dvecs_codec_h__

#ifndef __veccodec_single_header__
#include "veccodec.h"
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <fstream>

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @name Dvecs
 * @{
 */


/*!
 * Load a dvecs file, output in a double buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Load the file in a single row.
 * Should not be used for loading files twice larger than RAM.
 * 
 * @param[out] vectors Upon success, points to a newly allocated buffer that contains the vectors. Must be NULL.
 * @param[out] vector_dim Dimension of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[out] vector_count Number of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[in] file_name File to read.
 * @param[in] allocator Function called for allocation (default std malloc).
 * 
 */
void    veccodec_load_dvecs (double*& vectors,size_t& vector_dim,size_t& vector_count,const char* file_name,void*(*allocator)(size_t)=malloc);

/*!
 * Load a dvecs file header, and compute dimension and feature count.
 * This version assumes that all vectors have the same dimension.
 *
 * @param[in,out] in Input stream.
 * @param[out] vector_dim Dimension of vectors. Must be 0.
 * @param[out] vector_count Number of vectors. Must be 0.
 * @return The file size.
 * 
 */
size_t    veccodec_read_dvecs_header (std::istream& in,size_t& vector_dim,size_t& vector_count);

/*!
 * Decode vectors for dvecs buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * @param[out] vectors If not NULL, double buffer where vectors are saved. 
 * @param[in] vector_dim Dimension of vectors. All vectors are expected to have this dimension.
 * @param[in] vector_count Number of vectors to decode.
 * @param[in] buffer Input buffer. If NULL, this function can be used to compute the size of this buffer.
 * re vectors are encoded.
 * @param[in] buffer_size Buffer size.
 * @return The size of double buffer.
 * 
 */
size_t  veccodec_decode_dvecs (double* vectors,size_t vector_dim,size_t vector_count,const uint8_t* buffer,size_t buffer_size);

/*!
 * Save a dvecs file, intput from a double buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Save the file in a single row.
 * Should not be used for saving files twice larger than RAM.
 * 
 * @param[in] vectors Vectors
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @param[in] file_name file to write 
 * @param[in] chunk_size Data is written to disk by chunks, to minimize I/O. This parameter selects the size of these chunks.
 * 
 */
void    veccodec_save_dvecs (const double* vectors,const size_t vector_dim,const size_t vector_count,const char* file_name,size_t chunk_size=VECCODEC_DEFAULT_FILE_CHUNK_SIZE);

/*!
 * Encode vectors into a buffer with dvecs format.
 * This version assumes that all vectors have the same dimension.
 *
 * @param[out] buffer If not NULL, vectors are encoded in this buffer. Otherwise, this function can be used for computing the size of encoded vectors.
 * @param[in] buffer_size Size of the buffer. The function will throw an exception if the buffer is not large enough. Ignored when buffer is NULL.
 * @param[in] vectors Vectors   
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @return The size of encoded vectors.
 * 
 */
size_t    veccodec_encode_dvecs (uint8_t* buffer,size_t buffer_size,const double* vectors,size_t vector_dim,size_t vector_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/** @} */

/** 
 * @file hvec8_codec.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */

/** 
 * @addtogroup veccodec
 * @{
 */

#ifndef __hvec8_codec_h__
#define __hvec8_codec_h__

#ifndef __veccodec_single_header__
#include "veccodec.h"
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <fstream>

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @name Hvec8
 * @{
 */


/*!
 * Load a hvec8 file, output in a uint8_t buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Load the file in a single row.
 * Should not be used for loading files twice larger than RAM.
 * 
 * @param[out] vectors Upon success, points to a newly allocated buffer that contains the vectors. Must be NULL.
 * @param[out] vector_dim Dimension of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[out] vector_count Number of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[in] file_name File to read.
 * @param[in] allocator Function called for allocation (default std malloc).
 * 
 */
void    veccodec_load_hvec8 (uint8_t*& vectors,size_t& vector_dim,size_t& vector_count,const char* file_name,void*(*allocator)(size_t)=malloc);

/*!
 * Load a hvec8 file header, and compute dimension and feature count.
 * This version assumes that all vectors have the same dimension.
 *
 * @param[in,out] in Input stream.
 * @param[out] vector_dim Dimension of vectors. Must be 0.
 * @param[out] vector_count Number of vectors. Must be 0.
 * @return The file size.
 * 
 */
size_t    veccodec_read_hvec8_header (std::istream& in,size_t& vector_dim,size_t& vector_count);

/*!
 * Decode vectors for hvec8 buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * @param[out] vectors If not NULL, uint8_t buffer where vectors are saved. 
 * @param[in] vector_dim Dimension of vectors. All vectors are expected to have this dimension.
 * @param[in] vector_count Number of vectors to decode.
 * @param[in] buffer Input buffer. If NULL, this function can be used to compute the size of this buffer.
 * re vectors are encoded.
 * @param[in] buffer_size Buffer size.
 * @return The size of uint8_t buffer.
 * 
 */
size_t  veccodec_decode_hvec8 (uint8_t* vectors,size_t vector_dim,size_t vector_count,const uint8_t* buffer,size_t buffer_size);

/*!
 * Save a hvec8 file, intput from a uint8_t buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Save the file in a single row.
 * Should not be used for saving files twice larger than RAM.
 * 
 * @param[in] vectors Vectors
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @param[in] file_name file to write 
 * @param[in] chunk_size Data is written to disk by chunks, to minimize I/O. This parameter selects the size of these chunks.
 * 
 */
void    veccodec_save_hvec8 (const uint8_t* vectors,const size_t vector_dim,const size_t vector_count,const char* file_name,size_t chunk_size=VECCODEC_DEFAULT_FILE_CHUNK_SIZE);

/*!
 * Encode vectors into a buffer with hvec8 format.
 * This version assumes that all vectors have the same dimension.
 *
 * @param[out] buffer If not NULL, vectors are encoded in this buffer. Otherwise, this function can be used for computing the size of encoded vectors.
 * @param[in] buffer_size Size of the buffer. The function will throw an exception if the buffer is not large enough. Ignored when buffer is NULL.
 * @param[in] vectors Vectors   
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @return The size of encoded vectors.
 * 
 */
size_t    veccodec_encode_hvec8 (uint8_t* buffer,size_t buffer_size,const uint8_t* vectors,size_t vector_dim,size_t vector_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/** @} */

/** 
 * @file siftgeo_codec.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */

/** 
 * @addtogroup veccodec
 * @{
 */

#ifndef __siftgeo_codec_h__
#define __siftgeo_codec_h__

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <fstream>

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @name Siftgeo
 * @{
 */

/*!
 * Load a siftgeo file.
 * 
 * Load the file in a single row.
 * Should not be used for loading files twice larger than RAM.
 * 
 * Coordinates format: 9 float values per descriptor:
 *  x, y, scale, angle, affine matrix component 1x1,1x2,2x1,2x2, cornerness
 * 
 * @param[out] coords Upon success, points to a newly allocated buffer that contains the coordinates. Must be NULL.
 * @param[out] descriptors Upon success, points to a newly allocated buffer that contains the descriptors. Must be NULL.
 * @param[out] descriptor_dim Dimension of descriptors
 * @param[out] descriptor_count Number of descriptors
 * @param[in] file_name file to read
 * @param[in] allocator Function called for allocation (default std malloc).
 * 
 */
void    veccodec_load_siftgeo (float*& coords,uint8_t*& descriptors,size_t& descriptor_dim,size_t& descriptor_count,const char* file_name,void*(*allocator)(size_t)=malloc);

/*!
 * Load a siftgeo file header, and compute dimension and feature count.
 * This version assumes that all descriptors have the same dimension.
 *
 * @param[in,out] in Input stream.
 * @param[out] descriptor_dim Dimension of descriptors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[out] descriptor_count Number of descriptors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @return The file size.
 * 
 */
size_t    veccodec_read_siftgeo_header (std::istream& in,size_t& descriptor_dim,size_t& descriptor_count);

/*!
 * Decode coords & descriptors from a siftgeo buffer.
 * This version assumes that all descriptors have the same dimension.
 * 
 * @param[out] coords If not NULL, float buffer where coordinates are saved. If NULL, this function can be used to compute the size of this buffer.
 * @param[out] descriptors If not NULL, buffer where descriptors are saved. If NULL, this function can be used to compute the size of this buffer.
 * @param[in] descriptor_dim Dimension of descriptors. All descriptors are expected to have this dimension.
 * @param[in] descriptor_count Number of descriptors to decode.
 * @param[in] buffer Buffer where descriptors are encoded.
 * @param[in] buffer_size Buffer size.
 * @return The size of float buffer.
 * 
 */
size_t  veccodec_decode_siftgeo (float* coords,uint8_t* descriptors,size_t descriptor_dim,size_t descriptor_count,const uint8_t* buffer,size_t buffer_size);

/*!
 * Save a siftgeo file.
 * This version assumes that all descriptors have the same dimension.
 * 
 * Save the file in a single row.
 * Should not be used for saving files twice larger than RAM.
 * 
 * @param[in] coords Coordinates
 * @param[in] descriptors Descriptors
 * @param[in] descriptor_dim Dimension of descriptors
 * @param[in] descriptor_count Number of descriptors
 * @param[in] file_name file to write 
 * 
 */
void    veccodec_save_siftgeo (const float* coords,const uint8_t* descriptors,const size_t descriptor_dim,const size_t descriptor_count,const char* file_name);

/*!
 * Encode descriptors into a buffer with siftgeo format.
 * This version assumes that all descriptors have the same dimension.
 *
 * @param[out] buffer If not NULL, descriptors and coordinates are encoded in this buffer. Otherwise, this function can be used for computing the size of encoded data.
 * @param[in] buffer_size Size of the buffer. The function will throw an exception if the buffer is not large enough. Ignored when buffer is NULL.
 * @param[in] coords Coordinates
 * @param[in] descriptors Descriptors   
 * @param[in] descriptor_dim Dimension of descriptors
 * @param[in] descriptor_count Number of descriptors
 * @return The size of encoded descriptors.
 * 
 */
size_t    veccodec_encode_siftgeo (uint8_t* buffer,size_t buffer_size,const float* coords,const uint8_t* descriptors,size_t descriptor_dim,size_t descriptor_count);


/** @} */

#ifdef __cplusplus
}
#endif

#endif

/** @} */

/** 
 * @file matv4_codec.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */

/** 
 * @addtogroup veccodec
 * @{
 */

#ifndef __matv4_codec_h__
#define __matv4_codec_h__

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <fstream>

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @name Matlab version 4
 * @{
 */
// generic
    
/*!
 * Decode a matlab v4 header.
 *
 * @param[out] type Data type (see VECCODEC_FORMAT_MATV4_xxxx variables).
 * @param[out] var_name Buffer for variable name, with at least 0x100 chars (includint tailing '\0').
 * @param[out] vector_dim Dimension of vectors. 
 * @param[out] vector_count Number of vectors. 
 * @param[in] buffer Buffer.
 * @param[in] buffer_size Buffer size.
 * @param[in] skip_data If true, will returns a buffer position after the data.
 * @return buffer position.
 * 
 */
size_t    veccodec_decode_matv4_header (int& type,char* var_name,size_t& vector_dim,size_t& vector_count,const uint8_t* buffer,size_t buffer_size,bool skip_data=false);

/**
 * Create a new matv4 file.
 * 
 * @param out std::ofstream.
 * @param file_name File name.
 */
void    veccodec_ofstream_open_matv4 (std::ofstream& out,const char* file_name);

/**
 * Close a matv4 file.
 * @param out std::ofstream.
 */
void    veccodec_ofstream_close_matv4 (std::ofstream& out);

// double 

/*!
 * Load a double matrix from a matlab v4 file.
 * 
 * Load the file in a single row.
 * Should not be used for loading files twice larger than RAM.
 * This function is not efficient for files containing more than one element (build your own with decode_matv4_xxx functions).
 * 
 * @param[out] vectors Upon success, points to a newly allocated buffer that contains the vectors. Must be NULL.
 * @param[out] vector_dim Dimension of vectors. If zero, will be loaded. Otherwise, throws if different.
 * @param[out] vector_count Number of vectors. If zero, will be loaded. Otherwise, throws if different.
 * @param[in] file_name File to read.
 * @param[in] var_name Variable to read. If NULL, will load the very first variable.
 * @param[in] allocator Function called for allocation (default std malloc).
 * 
 */
void    veccodec_load_matv4_matrix_double (double*& vectors,size_t& vector_dim,size_t& vector_count,const char* file_name,const char* var_name=NULL,void*(*allocator)(size_t)=malloc);

/*!
 * Load a double from a matlab v4 file.
 * 
 * Load the file in a single row.
 * Should not be used for loading files twice larger than RAM.
 * This function is not efficient for files containing more than one element (build your own with decode_matv4_xxx functions).
 * 
 * @param[in] file_name File to read.
 * @param[in] var_name Variable to read. If NULL, will load the very first variable.
 * @return the double value.
 * 
 */
double    veccodec_load_matv4_double (const char* file_name,const char* var_name=NULL);
  
/*!
 * Decode a matlab v4 double matrix.
 *
 * @param[out] matrix Matrix buffer. If NULL, this function can be used to skip the data. 
 * @param[in] vector_dim Dimension of vectors. 
 * @param[in] vector_count Number of vectors. 
 * @param[in] buffer Buffer.
 * @param[in] buffer_size Buffer size.
 * @return buffer position.
 * 
 */
size_t    veccodec_decode_matv4_matrix_double (double* matrix,size_t vector_dim,size_t vector_count,const uint8_t* buffer,size_t buffer_size);

/*!
 * Save a double matrix to a matlab v4 file.
 * 
 * Save the file in a single row.
 * Should not be used for saving files twice larger than RAM.
 * 
 * @param[in] vectors Vectors
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @param[in] var_name Variable name.
 * @param[in] file_name file to write 
 * 
 */
void    veccodec_save_matv4_matrix_double (const double* vectors,const size_t vector_dim,const size_t vector_count,const char* file_name,const char* var_name="X");

/*!
 * Encode a matlab v4 block with a double matrix.
 *
 * @param[out] buffer If not NULL, vectors are encoded in this buffer. Otherwise, this function can be used for computing the size of encoded vectors.
 * @param[in] buffer_size Size of the buffer. The function will throw an exception if the buffer is not large enough. Ignored when buffer is NULL.
 * @param[in] vectors Vectors   
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @return The size of encoded vectors.
 * 
 */
size_t    veccodec_encode_matv4_matrix_double (uint8_t* buffer,size_t buffer_size,const double* vectors,size_t vector_dim,size_t vector_count);

/**
 * Add a double to a matv4 file.
 * 
 * @param out std::ofstream.
 * @param value Double.
 * @param var_name Variable name.
 */
void    veccodec_ofstream_write_matv4_double (std::ofstream& out,const double value,const char* var_name);

/**
 * Add a double matrix to a matv4 file.
 * 
 * @param out std::ofstream.
 * @param vectors Double matrix.
 * @param vector_dim Matrix column size.
 * @param vector_count Matrix row size.
 * @param var_name Variable name.
 */
void    veccodec_ofstream_write_matv4_matrix_double (std::ofstream& out,const double* vectors,const size_t vector_dim,const size_t vector_count,const char* var_name);

// float

/*!
 * Load a float matrix from a matlab v4 file.
 * 
 * Load the file in a single row.
 * Should not be used for loading files twice larger than RAM.
 * This function is not efficient for files containing more than one element (build your own with decode_matv4_xxx functions).
 * 
 * @param[out] vectors Upon success, points to a newly allocated buffer that contains the vectors. Must be NULL.
 * @param[out] vector_dim Dimension of vectors. If zero, will be loaded. Otherwise, throws if different.
 * @param[out] vector_count Number of vectors. If zero, will be loaded. Otherwise, throws if different.
 * @param[in] file_name File to read.
 * @param[in] var_name Variable to read. If NULL, will load the very first variable.
 * @param[in] allocator Function called for allocation (default std malloc).
 * 
 */
void    veccodec_load_matv4_matrix_float (float*& vectors,size_t& vector_dim,size_t& vector_count,const char* file_name,const char* var_name=NULL,void*(*allocator)(size_t)=malloc);

/*!
 * Load a float from a matlab v4 file.
 * 
 * Load the file in a single row.
 * Should not be used for loading files twice larger than RAM.
 * This function is not efficient for files containing more than one element (build your own with decode_matv4_xxx functions).
 * 
 * @param[in] file_name File to read.
 * @param[in] var_name Variable to read. If NULL, will load the very first variable.
 * @return the float value.
 * 
 */
float    veccodec_load_matv4_float (const char* file_name,const char* var_name=NULL);
   
/*!
 * Decode a matlab v4 float matrix.
 *
 * @param[out] matrix Matrix buffer. If NULL, this function can be used to skip the data. 
 * @param[in] vector_dim Dimension of vectors. 
 * @param[in] vector_count Number of vectors. 
 * @param[in] buffer Buffer.
 * @param[in] buffer_size Buffer size.
 * @return buffer position.
 * 
 */
size_t    veccodec_decode_matv4_matrix_float (float* matrix,size_t vector_dim,size_t vector_count,const uint8_t* buffer,size_t buffer_size);

/*!
 * Encode a matlab v4 block with a float matrix.
 *
 * @param[out] buffer If not NULL, vectors are encoded in this buffer. Otherwise, this function can be used for computing the size of encoded vectors.
 * @param[in] buffer_size Size of the buffer. The function will throw an exception if the buffer is not large enough. Ignored when buffer is NULL.
 * @param[in] vectors Vectors   
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @return The size of encoded vectors.
 * 
 */
size_t    veccodec_encode_matv4_matrix_float (uint8_t* buffer,size_t buffer_size,const float* vectors,size_t vector_dim,size_t vector_count);

/*!
 * Save a float matrix to a matlab v4 file.
 * 
 * Save the file in a single row.
 * Should not be used for saving files twice larger than RAM.
 * 
 * @param[in] vectors Vectors
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @param[in] var_name Variable name.
 * @param[in] file_name file to write 
 * 
 */
void    veccodec_save_matv4_matrix_float (const float* vectors,const size_t vector_dim,const size_t vector_count,const char* file_name,const char* var_name="X");

/**
 * Add a float matrix to a matv4 file.
 * 
 * @param out std::ofstream.
 * @param vectors Float matrix.
 * @param vector_dim Matrix column size.
 * @param vector_count Matrix row size.
 * @param var_name Variable name.
 */
void    veccodec_ofstream_write_matv4_matrix_float (std::ofstream& out,const float* vectors,const size_t vector_dim,const size_t vector_count,const char* var_name);

/**
 * Add a double to a matv4 file.
 * 
 * @param out std::ofstream.
 * @param value Double.
 * @param var_name Variable name.
 */
void    veccodec_ofstream_write_matv4_float (std::ofstream& out,const float value,const char* var_name);

/** @} */


#ifdef __cplusplus
}
#endif

#endif

/** @} */

/** 
 * @file m_codec.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */

/** 
 * @addtogroup veccodec
 * @{
 */

#ifndef __m_codec_h__
#define __m_codec_h__

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <fstream>

#ifdef __cplusplus
extern "C" {
#endif

/** 
 * @name M-file double
 * @{
 */

/*!
 * Load a m file, output in a double buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Load the file in a single row.
 * Should not be used for loading files twice larger than RAM.
 * 
 * @param[out] vectors Upon success, points to a newly allocated buffer that contains the vectors. Must be NULL.
 * @param[out] vector_dim Dimension of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[out] vector_count Number of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[in] file_name File to read.
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @param[in] allocator Function called for allocation (default std malloc).
 * 
 */
void    veccodec_load_m_double (double*& vectors,size_t& vector_dim,size_t& vector_count,const char* file_name,char vsep=',',char lsep='\n',void*(*allocator)(size_t)=malloc);

/*!
 * Decode vectors for m buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * @param[out] vectors If not NULL, double buffer where vectors are saved. If NULL, this function can be used to compte the dimension and count of vectors.
 * @param[in] vector_dim Dimension of vectors. If vectors is not NULL, it will be checked. Otherwise, it will contain the dimension of vectors.
 * @param[in] vector_count Number of vectors. If vectors is not NULL, it will be checked. Otherwise, it will contain the number of vectors.
 * @param[in] buffer Input buffer. Must not be NULL.
 * @param[in] buffer_size Buffer size.
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @return The size of vectors buffer.
 * 
 */
size_t  veccodec_decode_m_double (double* vectors,size_t& vector_dim,size_t& vector_count,const uint8_t* buffer,size_t buffer_size,char vsep=',',char lsep='\n');

/*!
 * Save a vtext file, intput from a double buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Save the file in a single row.
 * Should not be used for saving files twice larger than RAM.
 * 
 * @param[in] vectors Vectors
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @param[in] format Encoding format (default "%f")
 * @param[in] file_name file to write 
 * 
 */
void    veccodec_save_m_double (const double* vectors,const size_t vector_dim,const size_t vector_count,const char* file_name,char vsep=',',char lsep='\n',const char* format="%f");

/*!
 * Encode vectors into a buffer with vtext format.
 * This version assumes that all vectors have the same dimension.
 *
 * @param[out] buffer If not NULL, vectors are encoded in this buffer. Otherwise, this function can be used for computing the size of encoded vectors.
 * @param[in] buffer_size Size of the buffer. The function will throw an exception if the buffer is not large enough. Ignored when buffer is NULL.
 * @param[in] vectors Vectors   
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @param[in] format Encoding format (default "%f")
 * @return The size of encoded vectors.
 * 
 */
size_t    veccodec_encode_m_double (uint8_t* buffer,size_t buffer_size,const double* vectors,size_t vector_dim,size_t vector_count,char vsep=',',char lsep='\n',const char* format="%f");

/** 
 * @}
 */


/** 
 * @name M-file float
 * @{
 */

/*!
 * Load a m file, output in a float buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Load the file in a single row.
 * Should not be used for loading files twice larger than RAM.
 * 
 * @param[out] vectors Upon success, points to a newly allocated buffer that contains the vectors. Must be NULL.
 * @param[out] vector_dim Dimension of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[out] vector_count Number of vectors. If zero, will be detected (if possible). Otherwise, throws if different.
 * @param[in] file_name File to read.
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @param[in] allocator Function called for allocation (default std malloc).
 * 
 */
void    veccodec_load_m_float (float*& vectors,size_t& vector_dim,size_t& vector_count,const char* file_name,char vsep=',',char lsep='\n',void*(*allocator)(size_t)=malloc);

/*!
 * Decode vectors for m buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * @param[out] vectors If not NULL, float buffer where vectors are saved. If NULL, this function can be used to compte the dimension and count of vectors.
 * @param[in] vector_dim Dimension of vectors. If vectors is not NULL, it will be checked. Otherwise, it will contain the dimension of vectors.
 * @param[in] vector_count Number of vectors. If vectors is not NULL, it will be checked. Otherwise, it will contain the number of vectors.
 * @param[in] buffer Input buffer. Must not be NULL.
 * @param[in] buffer_size Buffer size.
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @return The size of vectors buffer.
 * 
 */
size_t  veccodec_decode_m_float (float* vectors,size_t& vector_dim,size_t& vector_count,const uint8_t* buffer,size_t buffer_size,char vsep=',',char lsep='\n');

/*!
 * Save a vtext file, intput from a float buffer.
 * This version assumes that all vectors have the same dimension.
 * 
 * Save the file in a single row.
 * Should not be used for saving files twice larger than RAM.
 * 
 * @param[in] vectors Vectors
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @param[in] format Encoding format (default "%f")
 * @param[in] file_name file to write 
 * 
 */
void    veccodec_save_m_float (const float* vectors,const size_t vector_dim,const size_t vector_count,const char* file_name,char vsep=',',char lsep='\n',const char* format="%f");

/*!
 * Encode vectors into a buffer with vtext format.
 * This version assumes that all vectors have the same dimension.
 *
 * @param[out] buffer If not NULL, vectors are encoded in this buffer. Otherwise, this function can be used for computing the size of encoded vectors.
 * @param[in] buffer_size Size of the buffer. The function will throw an exception if the buffer is not large enough. Ignored when buffer is NULL.
 * @param[in] vectors Vectors   
 * @param[in] vector_dim Dimension of vectors
 * @param[in] vector_count Number of vectors
 * @param[in] vsep Character separating values (default ',')
 * @param[in] lsep Character separating lines (default '\n')
 * @param[in] format Encoding format (default "%f")
 * @return The size of encoded vectors.
 * 
 */
size_t    veccodec_encode_m_float (uint8_t* buffer,size_t buffer_size,const float* vectors,size_t vector_dim,size_t vector_count,char vsep=',',char lsep='\n',const char* format="%f");

/** @} */


#ifdef __cplusplus
}
#endif

#endif

/** @} */

