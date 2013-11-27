#include "VirtualMachine.h"
#include <fstream>
#include <iostream>

#include "zip.h"
#include <cstdlib>
#include <stdio.h>

using namespace std;

/**
 * Constructor for VirtualMachine. Nothing is specially allocated for this, so nothing is deleted here.
 */
VirtualMachine::VirtualMachine() : rtZip(NULL), jceZip(NULL), jsseZip(NULL) {
	string rt = string(getenv("JAVA_HOME"))+"/jre/lib/rt.jar";
	int error = 0;
	rtZip = zip_open(rt.c_str(), 0, &error);
	if(!rtZip) {
		throw error;
	}
	
	string jce = string(getenv("JAVA_HOME"))+"/jre/lib/jce.jar";
	jceZip = zip_open(jce.c_str(), 0, &error);
	if(!jceZip) {
		throw error;
	}
	
	string jsse = string(getenv("JAVA_HOME"))+"/jre/lib/jsse.jar";
	jsseZip = zip_open(jsse.c_str(), 0, &error);
	if(!jsseZip) {
		throw error;
	}
}

/**
 * Destructor for a VirtualMachine. Deletes all of the class objects that have been loaded.
 */
VirtualMachine::~VirtualMachine() {
	zip_close(rtZip); // Get rid of zip file
	for(map<string,ClassFile*>::iterator it = classes.begin(); it != classes.end(); it++) {
		delete it->second;
	}
}

/**
 * Gets the representation of a class file. If it has not yet been loaded, loads it and initializes it.
 */
ClassFile& VirtualMachine::getClass(string name) {
	if(classes.count(name)) {
		return *(classes[name]);
	} else {
		std::string className = name + ".class";
		struct zip_file* classZipFile = zip_fopen(rtZip, className.c_str(), 0);
		//I could definitely do this better.
		if(!classZipFile) {
			classZipFile = zip_fopen(jceZip, className.c_str(), 0);
			if(!classZipFile) {
				classZipFile = zip_fopen(jsseZip, className.c_str(), 0);
				if(!classZipFile) {
					cout << name << endl;
					throw zip_strerror(rtZip);
				}
			}
		}
#define BUFFER_SIZE (1 << 10)
		char buffer[BUFFER_SIZE];
		std::string fileStr = "";
		int bytes = 0;
		while((bytes = zip_fread(classZipFile, buffer, BUFFER_SIZE)) > 0) {
			fileStr = fileStr + std::string(buffer, bytes);
		}
		zip_fclose(classZipFile); //Get rid of file memory
		std::stringstream filestream(fileStr);
		//f.exceptions(fstream::eofbit | fstream::failbit | fstream::badbit);
		ClassFile* cf = new ClassFile(*this,filestream);
		//cout << "Constructed class file object for " + name << endl;
		classes[name] = cf;
		cf->initialize();
		return *cf;
	
	}
	throw "help";
}

/**
 * Tells the virtual machine what its main class should be.
 */
void VirtualMachine::setMainClass(string name) {
	main = &(getClass(name));
}

/**
 * Runs the main function of the main class of the virtual machine
 */
void VirtualMachine::runMain() {
	cout << classes.size() << endl;
}
