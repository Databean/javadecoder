#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <map>
#include <string>
#include "ClassFile.h"
#include "ClassInstance.h"
#include <inttypes.h>

#include <zip.h>

class ClassFile;
class ClassInstance;

/**
 * This class represents the entire Virtual Machine, with all of its classes, and class instances.
 */
class VirtualMachine {
public:
	VirtualMachine();
	virtual ~VirtualMachine();
	
	virtual void setMainClass(std::string name);
	virtual void runMain();
	
	virtual ClassFile& getClass(std::string name);
	
	virtual ClassInstance& getClassInstance(uint32_t index) { return *(instances[index]); }
	virtual JavaArray& getJavaArray(uint32_t index) { return *(arrays[index]); }
private:
	struct zip* rtZip;
	struct zip* jceZip;
	struct zip* jsseZip;
	ClassFile* main;
	std::map<std::string,ClassFile*> classes;
	std::map<uint32_t,ClassInstance*> instances;
	std::map<uint32_t,JavaArray*> arrays;
};

#endif