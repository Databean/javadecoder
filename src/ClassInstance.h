#ifndef CLASS_INSTANCE_H
#define CLASS_INSTANCE_H

#include "ClassFile.h"
#include <map>
#include <string>
#include <inttypes.h>

class ClassFile;

class ClassInstance {
public:
	virtual ~ClassInstance() {}
	
	template<class T> T getVariable(std::string name) { return reinterpret_cast<T&>(vars[name]); }
	template<class T> void setVariable(std::string name,const T& value) { vars[name] = reinterpret_cast<uint64_t&>(value); }
	
	friend class ClassFile;
	
protected:
	ClassInstance(ClassFile* cf) { classFile = cf; }
	
	ClassFile* classFile;
	std::map<std::string,uint64_t> vars;
};

class JavaArray {
public:
	virtual ~JavaArray() {
		delete[] storage;
	}
	
	template<class T> T getValue(uint32_t index) {
		if(index>=length) {
			return 0;
		} else {
			return reinterpret_cast<T&>(storage[index]);
		}
	}
	template<class T> void setValue(uint32_t index,T value) {
		if(index>=length) { return; }
		storage[index] = reinterpret_cast<uint64_t&>(value);
	}
	
protected:
	JavaArray(uint32_t length) {
		this->length = length;
		storage = new uint64_t[length];
	}
	
	uint64_t* storage;
	uint32_t length;
};
#endif