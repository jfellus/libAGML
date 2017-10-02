#define __string_tools_single_header__
/** 
 * @file string_tools.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */

/** 
 * \defgroup string_tools [library] string_tools: String format and parsing, file and path names operations (C/A+)
 * \ingroup core
 */


/** 
 * @file string_format.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */

/** 
 * @addtogroup string_tools
 * @{
 */

#ifndef __string_format_h__
#define __string_format_h__

#include <string>
#include <stdarg.h>
#include <stdint.h>
#include <iostream>

#ifdef __cplusplus
extern "C" {
#endif

    
/** 
 * @name String format
 * @{
 */


/*!
 * Build a std::string using sprintf format.
 * 
 * Example:
 * \code{.cpp}
 * cout << string_format("The string '%s' has %d characters","hello",5);
 * \endcode
 * 
 * @param fmt String format 
 * @return std::string
 *  
 */
std::string string_format(const char* fmt, ...);

/*!
 * Same as \ref string_format.
 *  
 */
std::string string_format_va_list(const char* fmt, va_list);

/*!
 * Build a std::string for error messages.
 * 
 * @param file File name
 * @param line Line number
 * @param func Function name (or NULL)
 * @param fmt String format 
 * @return std::string
 *  
 */
std::string string_error(const char* file,const int line,const char* func,const char* fmt, ...);

/**
 * Create a string with the following format:
 * 
 *    [days] day [hour]:[minutes]:[seconds]
 * 
 * Note that the format is reduced if time is short. For instance, if duration is
 * less than one day, days are omitted, if duration is less than one hour,
 * hours are ommitted, etc.
 * 
 * 
 * @param duration Duration in seconds
 * @return std::string
 */
std::string string_format_duration(int64_t duration);

/**
 * Create a string with the following format:
 * 
 *    [days] day [hour]:[minutes]:[seconds]:[miliseconds]
 * 
 * Note that the format is reduced if time is short. For instance, if duration is
 * less than one day, days are omitted, if duration is less than one hour,
 * hours are ommitted, etc.
 * 
 * 
 * @param duration Duration in seconds. Value after coma are sub-second durations,
 * ie 0.500 = half a second = 500 milliseconds
 * @return std::string
 */
std::string string_format_duration_msec(double duration);

//! Structure for string_format_remaining_time()
class string_format_remaining_time_struct;

/**
 * The purpose of this function is to create a message indicating an estimation
 * of the remaining time for some procedure.
 * It is assumed that the procedure has max_count step of near equal duration.
 * 
 * It format something like:
 *     [current_count]/[max_count] Done. Elapsed: [time elasped] Average: 
 * [average duration for one operation]. Remaining: [estimation of remaining time]
 * 
 * Note that the function will not always return a message: it depends on min_per_diff
 * and delay. That way, you can display a message only when something happens (progress 
 * and some time)
 * 
 * @param message Contains the message with remaining time
 * @param config Contains internal data. If NULL, it is initialized with current time when current_count == 0.
 * If NULL and current_count != 0, nothing happens.
 * If not NULL, it is deleted and NULLed when current_count >= max_count.
 * @param current_count The number of operations currently done
 * @param max_count The maximum number of operations. Ignored if current_count > 0
 * @param min_per_diff This is the required minimum percentage done to trigger a message. Ignored if current_count > 0
 * @param delay This is the minimum time between two messages. Ignored if current_count > 0
 * @return true if message was updated
 */
bool string_format_remaining_time(
        std::string& message,string_format_remaining_time_struct*& config,
        size_t current_count=~0,size_t max_count=~0,double min_per_diff=1,double delay=10);

/** @} */

/** 
 * @name Macros for formatting error messages.
 * @{
 */

/*!
 * Build a std::string for error messages containing current file name, line number, and function name.
 * \see string_error 
 */
#define string_error_func(msg) (string_error(__FILE__,__LINE__,__func__,msg))
/*!
 * Same as \ref string_error_func, but with 1 argument.
 * 
 * Example:
 * \code{.cpp}
 * throw std::runtime_error(string_error_func("An error occurred"));
 * \endcode
 * 
 */
#define string_error_func1(msg,arg1) (string_error(__FILE__,__LINE__,__func__,msg,arg1))
/*!
 * Same as \ref string_error_func, but with 2 arguments.
 * 
 * Example:
 * \code{.cpp}
 * throw std::invalid_argument(string_error_func1("'%s' operator not found",name));
 * \endcode
 * 
 */
#define string_error_func2(msg,arg1,arg2) (string_error(__FILE__,__LINE__,__func__,msg,arg1,arg2))
/*!
 * Same as \ref string_error_func, but with 3 arguments.
 */
#define string_error_func3(msg,arg1,arg2,arg3) (string_error(__FILE__,__LINE__,__func__,msg,arg1,arg2,arg3))
/*!
 * Same as \ref string_error_func, but with 4 arguments.
 */
#define string_error_func4(msg,arg1,arg2,arg3,arg4) (string_error(__FILE__,__LINE__,__func__,msg,arg1,arg2,arg3,arg4))

/** @} */

#ifdef __cplusplus
}
#endif


#endif

/** @} */

/** 
 * @file string_parse.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */

/** 
 * @addtogroup string_tools
 * @{
 */

#ifndef __string_parse_h__
#define __string_parse_h__

#include <stdint.h>
#include <string>
#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

    
/** 
 * @name String parsing
 * @{
 */
    
/**
 * Test if a string starts with a specific pattern.     
 * 
 * @param[in] str String to parse.
 * @param[in] pattern Pattern to check.
 * @return True if the pattern is found.
 */
bool string_starts_with(const char* str,const char* pattern);

/**
 * Test if a string ends with a specific pattern.     
 * 
 * @param[in] str String to parse.
 * @param[in] pattern Pattern to check.
 * @return True if the pattern is found.
 */
bool string_ends_with(const char* str,const char* pattern);

/*! 
 * Convert a string to an integer.
 * 
 * @param str The string to parse
 * @param min Minimum expected value
 * @param max Maximum expected value
 * @param base Integer base (default 10)
 * @return The integer
 * @throws std::invalid_argument if parsing fails; value is less than \a min; value is greater than \a max.
 *  
 */
int64_t string2int(const char* str,int64_t min=-2147483648,int64_t max=2147483647,int base=10);

/*! 
 * Convert a string to a double.
 * 
 * @param str The string to parse
 * @return The double
 * @throws std::invalid_argument if parsing fails, or if the number is not finite.
 *  
 */
double string2double(const char* str);

/**
 * @param str String to parse.
 * @return true if str only contains letters.
 */
bool    string_is_alpha(const char* str);

/**
 * @param str String to parse.
 * @return true if str only contains digits.
 */
bool    string_is_numeric(const char* str);

/**
 * @param str String to parse.
 * @return true if str starts with a letter, and then only digits and letters.
 */
bool    string_is_alphanum(const char* str);

/**
 * @param str String to parse.
 * @return true  if str starts with a letter, and then only digits, letters and '_'.
 */
bool    string_is_varname(const char* str);

/*! 
 * Split a string into a string list, according to a character delimiter.
 * 
 * @param str The string to split
 * @param delim The delimiter (default space)
 * @return a new list of strings
 *  
 */
std::vector<std::string> string_split(const char* str, char delim=' ');

/*! 
 * Split a string into a string list, according to a character delimiter.
 * Basically the same as \ref string_split except that it fills an already existing list.
 * Note: it does not empty the list.
 * 
 * @param elems The list to fill
 * @param str The string to split
 * @param delim The delimiter (default space)
 *  
 */
void string_split_list(std::vector<std::string> &elems, const char* str, char delim=' ');


/**
 * Split a string into a string list, according to usual syntaxic rules in programming languages.
 * The list is split into words, where a word can be:
 *   + A alpha numeric string (like C variables with letter, digits and '_')
 *   + A numeric string (like C float values, like 123.45e67)
 *   + A string separated by a single or double quote. The function processes backslashes ('\'): 
 * any '\\x' is converted to 'x'.
 * 
 * @param elems List appended by this function.
 * @param str The string to parse.
 * @param size String size
 */
void    string_tokenize (std::vector<std::string>& elems,const char* str,size_t size=~0);

/**
 * Return (if any) the first token in the string
 * @see string_tokenize
 * @param token Next token or empty string
 * @param str The string to parse
 * @param size String size
 * @return Position after token
 */
size_t  string_next_token (std::string& token,const char* str,size_t size=~0);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

/** @} */
/** 
 * @file string_file.h
 * @author Philippe-Henri Gosselin
 * @date 2014
 * @copyright CNRS
 */

/** 
 * @addtogroup string_tools
 * @{
 */

#ifndef __string_file_h__
#define __string_file_h__

#include <stddef.h>
#include <stdlib.h>
#include <vector>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

    
/** 
 * @name String I/O
 * @{
 */

/**
 * @param path A file, directory, link, ...
 * @return true if path exists
 */    
bool    string_path_exists (const char* path);    
    
/**
 * Create directories and required sub directories.
 * @param path Full directory path (ie not a file!)
 * @param mode Permissions (will be modified by process umask)
 */
void    string_path_create_directories(const char* path,size_t mode=0777);

/*!
 * Write an entire string in a file.
 * 
 * @param[in] str String to save
 * @param[in] file_name File name
 * @throws std::exception
 */
void    string_save (const std::string& str,const char* file_name);    

/*!
 * Write an entire buffer in a file.
 * 
 * @param[in] buffer Buffer. If NULL, juste create an empty file.
 * @param[in] file_name File name
 * @param[in] buffer_size Buffer size. If zero, the function computes the string size with strlen().
 * @throws std::exception
 */
void    string_save_buffer (const char* buffer,const char* file_name,size_t buffer_size=0);    


/*!
 * Load an entire file in a string.
 * 
 * @param[out] str String
 * @param[in] file_name File name
 * @throws std::exception
 */
void    string_load (std::string& str,const char* file_name);

/**
 * Load an entire text file line by line, and store each line in a list
 * 
 * NB: list is appended, not cleared ! 
 * 
 * @param list Appended list of strings (existing content is not removed)
 * @param file_name File name
 */
void    string_load_list(std::vector<std::string>& list,const char* file_name);

/*!
 * Load an entire file in a buffer.
 * 
 * @param[out] buffer Buffer
 * @param[in] file_name File name
 * @param[in] allocator Function called for allocation (default std malloc).
 * @return Buffer size
 * @throws std::exception
 */
size_t  string_load_buffer (char*& buffer,const char* file_name,void*(*allocator)(size_t)=malloc);

/*!
 * Load an entire file in a vector.
 * 
 * @param[out] buffer Vector buffer
 * @param[in] file_name File name
 * @throws std::exception
 */
void    string_load_vector (std::vector<char>& buffer,const char* file_name);

/** @} */

#ifdef __cplusplus
}
#endif


#endif

/** @} */

