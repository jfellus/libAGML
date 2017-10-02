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

#include "NodeLibrary.h"
#include <pthread.h>
#include "../util/utils.h"
#include "../util/array.h"
#include <dlfcn.h>
#include <dirent.h>
#include <string>

static bool bInited = false;
static array<void*> dl_handles;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

void node_library_add_dir(const std::string& path);


void node_library_init() {
	pthread_mutex_lock(&mut);
	if(!bInited) {
		dl_handles.add(dlopen(NULL, RTLD_GLOBAL | RTLD_NOW));

		if(file_is_directory(TOSTRING(home() << "/agml/"))) node_library_add(TOSTRING(home() << "/agml/"));
		if(file_is_directory(TOSTRING(home() << "/agml/lib"))) node_library_add(TOSTRING(home() << "/agml/lib"));
		if(file_is_directory(TOSTRING(home() << "/.agml/"))) node_library_add(TOSTRING(home() << "/.agml/"));

		if(getenv("AGML_PATH")) {
			char agml_path[2048]; strcpy(agml_path,getenv("AGML_PATH"));
			for(char* path = strtok(agml_path, ":"); path!=0; path=strtok(NULL, ":")) {
				if(file_is_directory(path)) node_library_add_dir(path);
				else node_library_add(path);
			}
		}

		bInited = true;
	}
	pthread_mutex_unlock(&mut);
}

void node_library_add_dir(const std::string& path) {
	DIR *dp;
	struct dirent *dirp;
	if((dp  = opendir(path.c_str())) == NULL) { ERROR("ERROR : Couldn't open nodeclasses directory " << path); return;}

	while ((dirp = readdir(dp)) != NULL) {
		if(str_ends_with(dirp->d_name, ".so"))
			node_library_add(TOSTRING(path << "/" << dirp->d_name));
	}
	closedir(dp);
}

void node_library_add(const std::string& shared_lib) {
	if(file_is_directory(shared_lib)) {node_library_add_dir(shared_lib); return;}

	void* h = dlopen(file_absolute_path(shared_lib).c_str(), RTLD_LAZY | RTLD_GLOBAL);

	if(!h) ERROR("ERROR : Couldn't load shared nodeclass library " << shared_lib << " : " << dlerror());
	else dl_handles.add(h);
}

Node* node_library_instanciate(const std::string& nodeclass) {
	if(!bInited) node_library_init();
	Node* (*f) () = 0;
	if(dl_handles.size()!=0) {
		const char* instanciator = TOSTRING("__agml_node_instanciator_"<<nodeclass).c_str();
		for(int i=dl_handles.size()-1; i>=0; i--) {
			f= (Node* (*)())((unsigned long) dlsym(dl_handles[i], instanciator));
			if(f) break;
		}
	}
	if(!f) {ERROR("Node Class not found : " << nodeclass); return 0;}
	return f();
}
