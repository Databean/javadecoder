#include "ClassFile.h"
#include "Constants.h"
#include "Util.h"
#include <iostream>
#include <stdexcept>
#include "Frame.h"

using std::istream;
using std::string;
using std::vector;
using std::runtime_error;
using Glib::ustring;


/*
 * ClassFile {
 * 		u4 magic;
 * 		u2 minor_version;
 * 		u2 major_version;
 * 		u2 constant_pool_count;
 * 		cp_info constant_pool[constant_pool_count-1];
 * 		u2 access_flags;
 * 		u2 this_class;
 * 		u2 super_class;
 * 		u2 interfaces_count;
 * 		u2 interfaces[interfaces_count];
 * 		u2 fields_count;
 * 		field_info fields[fields_count];
 * 		u2 methods_count;
 * 		method_info methods[methods_count];
 * 		u2 attributes_count;
 * 		attribute_info attributes[attributes_count];
 * }
 */

/**
 * Constructs the ClassFile, storing a reference to the VirtualMachine it is a part of
 * and reading all of its data from an input stream, which gets passed to the constructors
 * of its component members.
 */
ClassFile::ClassFile(VirtualMachine& vm,istream& file) try :
	vm(vm),
	magic(readIntUnsigned(file)),
	minor_version(readShortUnsigned(file)),
	major_version(readShortUnsigned(file)),
	constantPool(*this, file),
	access_flags(file),
	this_class(readShortUnsigned(file)),
	super_class(readShortUnsigned(file)),
	interfaces(buildInterfaces(file)),
	fields(*this, file),
	methods(*this, file),
	attributePool(*this, file) {
	
	
	clinit = NULL;
	if(magic != 0xCAFEBABE) {
		throw "not a class file!";
	}
	if(!constantPool.validate()) {
		throw runtime_error("Constant Pool did not validate.");
	}
	
	
	//find <init> and <clinit>
	for(u2 i = 0;i < methods.numMembers(); i++) {
		//TODO: valgrind complains when this isn't string(...)
		if(string(methods[i].getName()) == "<clinit>") {
			clinit = &(methods[i]);
		}
	}
} catch(...) {
	throw;
}

/**
 * Destroys the class file. Nothing directly allocates memory, and the field constructors
 * are called implicitly.
 */
ClassFile::~ClassFile() {
	
}

/**
 * Called in the constructer to build a vector of interface IDs given an input stream.
 */
vector<uint16_t> ClassFile::buildInterfaces(istream& in) {
	vector<uint16_t> ret(readShortUnsigned(in));
	for(uint16_t i = 0; i < ret.size(); i++) {
		ret[i] = readShortUnsigned(in);
	}
	return ret;
}

/**
 * Makes the class file ready for usage by the VirtualMachine.
 * TODO: This is currently being used as a quick-and-dirty way to test a large number of class files
 * by loading in every single class file mentioned as a constant. This loads in way too many class files
 * for real usage, so get rid of this later.
 * TODO: Make this run the <cl_init> method.
 */
void ClassFile::initialize() {
	for(u2 i=1;i<constantPool.getNumElements();i++) {
		Constant& c = constantPool[i];
		if(&c != NULL && constantPool.isType<ConstantClassInfo>(i)) {
			ustring name = constantPool.get<ConstantClassInfo>(i).getClassName();
			if(name[0] == '[') {
				while(name[0] == '[') {
					name = name.substr(1);
				}
				// If we found a [, it means that there is an extra L before the class name.
				name = name.substr(1); 
				// There is also a semicolon after the class name in this case.
				name = name.substr(0, name.size() - 1);
			}
			//TODO: when i leave this as name != "", i get valgrind errors all over the place. I don't know if this is glib or if it's me.
			if(string(name) != "") {
				vm.getClass(name);
			}
		}
	}
	if(clinit != NULL) {
		
	}
}

/**
 * Gets the magic constant associated with this class file. If it's not 0xCAFEBABE, something has gone wrong.
 */
uint32_t ClassFile::getMagic() const {
	return magic;
}

/**
 * Returns the minor version of the java class file. Depends on the compiler that generated it.
 */
uint16_t ClassFile::getMinorVersion() const {
	return minor_version;
}

/**
 * Returns the major version of the java class file. Depends on the compiler that generated it.
 */
uint16_t ClassFile::getMajorVersion() const {
	return major_version;
}

/**
 * Returns the AccessFlags for this class file, showing who has which access privliges.
 */
AccessFlags& ClassFile::getAccessFlags() {
	return access_flags;
}

/**
 * Returns the const AccessFlags for this class file, showing who has access priviliges.
 */
const AccessFlags& ClassFile::getAccessFlags() const {
	return access_flags;
}

/**
 * Returns the constant pool in this class file.
 */
ConstantPool& ClassFile::getConstantPool() {
	return constantPool;
}

/**
 * Returns the const constant pool in this class file.
 */
const ConstantPool& ClassFile::getConstantPool() const {
	return constantPool;
}

/**
 * Returns the pool of fields in this class file.
 */
ClassMemberPool& ClassFile::getFields() {
	return fields;
}

/**
 * Returns the const pool of fields in this class file.
 */
const ClassMemberPool& ClassFile::getFields() const {
	return methods;
}

/**
 * Returns the pool of methods in this class file.
 */
ClassMemberPool& ClassFile::getMethods() {
	return methods;
}

/**
 * Returns the const pool of methods in this class file.
 */
const ClassMemberPool& ClassFile::getMethods() const {
	return methods;
}

/**
 * Returns the pool of attributes in this class file.
 */
AttributePool& ClassFile::getAttributes() {
	return attributePool;
}

/**
 * Returns the const pool of attributes in this class file.
 */
const AttributePool& ClassFile::getAttributes() const {
	return attributePool;
}

