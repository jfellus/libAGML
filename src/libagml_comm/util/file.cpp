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

#include "file.h"
#include "string.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ios>
#include "utils.h"
#include <pwd.h>
#include <fstream>

bool file_has_ext(const char* filename, const char* ext) {
	return str_ends_with(str_to_lower(filename),ext);
}

std::string file_basename(const std::string& file) {
	size_t i = file.rfind('/');
	if(i==std::string::npos) return file;
	return file.substr(i+1);
}

std::string file_dirname(const std::string& file) {
	size_t i = file.rfind('/');
	if(i==std::string::npos) return ".";
	else return file.substr(0,i);
}



void fcomeBackToLine(std::istream& f, int linesBefore) {
	while(linesBefore<0) {
		f.seekg(-2, std::ios_base::cur);
		if(!f.good()) {f.clear(); return;}
		int c = f.get();
		if(c=='\n') linesBefore++;
	}
}

bool file_exists(const std::string& filename) {
	  struct stat   buffer;
	  return (stat (filename.c_str(), &buffer) == 0);
}

bool file_is_directory(const std::string& filename) {
	struct stat   buffer;
	if(stat (filename.c_str(), &buffer) != 0) return false;
    return (S_ISDIR (buffer.st_mode));
}

std::string cwd() {
	char buf[1024];
	char* b = getcwd(buf, 1024);
	(void)b;
	std::string s = buf;
	return s;
}

std::string home() {
	struct passwd *pw = getpwuid(getuid());
	const char *homedir = pw->pw_dir;
	std::string s = homedir;
	return s;
}

std::string get_cur_exec_path() { char buf[1024]; size_t s = readlink("/proc/self/exe", buf, 1024); (void) s; return buf;}
std::string main_dir() { return file_dirname(get_cur_exec_path());}

std::string file_absolute_path(const std::string& path) {
	if(path.empty()) return "";
	char* s = realpath(path.c_str(), NULL);
	if(!s) {
		if(path[0]=='/') return path;
		return cwd() + "/" + path;
	}
	std::string ss = s;
	free(s);
	return ss;
}

std::string fgetlines(std::istream& f, int nblines) {
	std::string s;
	std::string buf;
	while(nblines) {
		getline(f, buf);
		if(!f.good()) return s;
		s += buf;
		s += '\n';
		nblines--;
	}
	return s;
}

void f_read_lines(const std::string& filename, std::vector<std::string>& lines) {
	std::ifstream f(filename.c_str());
	if(!f.good()) return;
	std::string s;
	while(getline(f, s).good()) lines.push_back(s);
	f.close();
}

void f_write_lines(const std::string& filename, const std::vector<std::string>& lines) {
	create_dir_for(filename);
	std::ofstream f(filename.c_str(), std::ofstream::trunc);
	for(uint i=0; i<lines.size(); i++) {
		f << lines[i] << "\n";
	}
	f.close();
}

void f_write_line(const std::string& filename, const std::string& s) {
	create_dir_for(filename);
	std::ofstream f(filename.c_str(), std::ofstream::trunc);
	f << s << "\n";
	f.close();
}

void f_append(const std::string& filename, const std::string& s) {
	create_dir_for(filename);
	std::ofstream f(filename.c_str(), std::ofstream::app);
	f << s << "\n";
	f.close();
}




std::string f_read_comments(std::istream& f) {
	  std::string comment = "",com;
	  char c;
	  while (f >> c && (c=='%' || c=='\n')) {
	    if (c == '%') {
	      std::getline(f, com);
	      comment += com;
	      comment += "\n";
	    }
	  }
	  f.seekg(-1, std::ios_base::cur);
	  return comment;
}

void f_write_comments(std::ostream& f, const std::string& comments) {
	if(comments.empty()) return;
	f << "%";
	std::string s = str_replace(comments, "\n", "\n%");
	while(s[s.size()-1]=='%') s = s.substr(0, s.size()-1);
	f << s;
	if(s[s.size()-1]!='\n') f << '\n';
}

void f_skipline(std::ifstream& f) {
	char c;
	while((c=f.get())!=-1 && c!='\n') {}
}

void create_dir_for(const std::string& filename) {
	shell(TOSTRING("mkdir -p " << file_dirname(filename).c_str()));
}


long file_get_modification_time(const std::string& filename) {
	struct stat attrib;
	stat(filename.c_str(), &attrib);
	return attrib.st_ctim.tv_sec;
}
