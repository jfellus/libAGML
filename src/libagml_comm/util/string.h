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

#ifndef COEOS_STRING_H_
#define COEOS_STRING_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <sstream>


#define MAX_PATH 1024

bool str_ends_with(const std::string& s, const std::string& suffix);
bool str_starts_with(const std::string& s, const std::string& prefix);
bool str_starts_with(const std::string& s, const std::string& prefix);
std::string str_after(const std::string& s, const std::string& prefix);
std::string str_before(const std::string& s, const std::string& suffix);
std::string str_between(const std::string& s, const std::string& prefix, const std::string& suffix);
std::string str_to_lower(const std::string& s);
std::string str_replace(std::string subject, const std::string& search, const std::string& replace);
bool str_has(const std::string& s, const std::string& needle);

std::string cwd();

void parse_keycode_str(const char* keycode_str, uint* key, uint* modifers);

#define TOSTRING(x) ((std::ostringstream&)(std::ostringstream().flush() << x)).str()
#define TOINT(x) toInt(x)
#define TOFLOAT(x) toFloat(x)

inline int toInt(const std::string& s) {
	int i = -1; std::istringstream iss(s); iss >> i; return i;
}

inline float toFloat(const std::string& s) {
	float i = -1; std::istringstream iss(s); iss >> i; return i;
}

template<typename T> std::string toString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template<typename T> void fromString(const std::string& s, T& v) {
    std::istringstream iss(s);
    iss >> v;
}

inline void fromString(const std::string& s, std::string& v) {v = s;}

std::string str_to_lower(const std::string& s);


#define JSON_P(prop, value) "\"" << prop << "\": \"" << toString(value) << "\""


std::string url_decode(const std::string& s);
std::string JSON_escape(const std::string& s);

std::string str_trim(const std::string& s);


/**
 * input : std::string
 * separator : char
 * s : variable name (each token will be available with this variable as a std::string)
 */
#define FOR_EACH_TOKEN(input, separator, s) 				\
		size_t ____i__=0,____j__=-1;									\
		std::string s;										\
		for(__for_each_token_init(____i__,____j__,s,input, separator);			\
				__for_each_token_cond(____i__);		\
				__for_each_token_update(____i__,____j__,s,input, separator))

inline int __for_each_token_init(size_t& i, size_t& j, std::string& s, const std::string& arg, char tok) {
	i=0;
	j=arg.find(tok, i);
	s = j==std::string::npos ? arg.substr(i,-1) : arg.substr(i,j-i);
	return 0;
}

inline int __for_each_token_cond(size_t& i) {
	return i!=std::string::npos;
}

inline int __for_each_token_update(size_t& i, size_t& j, std::string& s, const std::string& arg, char tok) {
	i = j==std::string::npos ? j : j+1;
	if(j!=std::string::npos) {
		j=arg.find(tok, i);
		s = j==std::string::npos ? arg.substr(i,-1) : arg.substr(i,j-i);
	}
	return 0;
}


void str_remove(std::string& s, const std::string& what);
std::string str_nth_occurence(const std::string& s, const std::string& needle, unsigned int N);
std::string str_param(const std::string& s, int iParam);


std::string str_align(const std::string& s, int nbchars);

#endif /* COEOS_STRING_H_ */
