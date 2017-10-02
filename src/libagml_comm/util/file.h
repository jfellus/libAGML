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

#ifndef FILE_H_
#define FILE_H_

#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>

bool file_has_ext(const char* filename, const char* ext);
inline bool file_has_ext(const std::string& filename, const char* ext) {  return file_has_ext(filename.c_str(), ext); }

inline std::string file_change_ext(const std::string& filename, const char* newext) {
	uint i = filename.rfind('.');
	if(i==std::string::npos) return filename + newext;
	return filename.substr(0, i) + newext;
}


inline bool file_is_absolute(const std::string& file) {
	return file[0]=='/';
}

std::string file_basename(const std::string& file);
std::string file_dirname(const std::string& file);

bool file_exists(const std::string& filename);
bool file_is_directory(const std::string& filename);
std::string file_absolute_path(const std::string& path);

void fcomeBackToLine(std::istream& f, int linesBefore);
std::string fgetlines(std::istream& f, int nblines);

void f_read_lines(const std::string& filename, std::vector<std::string>& lines);
void f_write_lines(const std::string& filename, const std::vector<std::string>& lines);
void f_write_line(const std::string& filename, const std::string& s);
void f_append(const std::string& filename, const std::string& s);


std::string f_read_comments(std::istream& f);
void f_write_comments(std::ostream& f, const std::string& comments);

template <class T> bool f_try_read(std::istream& f, const char* fmt, T& val) {
	int i;
	char c;
	for(i=0; fmt[i]!=0; i++) {
		if(fmt[i]=='%') {
			while((c = f.get())==' ' || c=='\t');
			if(c=='\n') throw(fmt);
			f.unget();
			i++;
			f >> val;
		} else {
			c = f.get();
			if(c!=fmt[i]) {f.unget(); throw(fmt);}
//			while((fmt[i]==' ' || fmt[i]=='\t') && (fmt[i+1]==' ' || fmt[i+1]=='\t')) i++;
		}
	}
	return true;
}

void f_skipline(std::ifstream& f);

std::string cwd();
std::string home();
std::string main_dir();

void create_dir_for(const std::string& filename);

long file_get_modification_time(const std::string& filename);


#endif /* FILE_H_ */
