#include "ClassMember.h"

#include <stdexcept>
#include "ClassFile.h"
#include "Util.h"

using std::istream;
using std::runtime_error;
using Glib::ustring;

/**
 * Constructor for AccessFlags that takes in the value.
 */
AccessFlags::AccessFlags(uint16_t value) : accessFlags(value) {
	
}

/**
 * Constructor for AccessFlags that reads the value in from an input stream.
 */
AccessFlags::AccessFlags(istream& in) {
	accessFlags = readShortUnsigned(in);
}

/**
 * Copy Constructor for AccessFlags.
 */
AccessFlags::AccessFlags(const AccessFlags& af) : accessFlags(af.getValue()) {
	
}

/**
 * Destructor for AccessFlags. Nothing is dynamically allocated for this, so nothing is destroyed here.
 */
AccessFlags::~AccessFlags() {
	
}

/**
 * Assignment operator for AccessFlags.
 */
const AccessFlags& AccessFlags::operator=(const AccessFlags& af) {
	accessFlags = af.getValue();
	return *this;
}

/**
 * Returns whether the public bit is set.
 */
bool AccessFlags::isPublic() const {
	return accessFlags & ACC_PUBLIC;
}

/**
 * Returns whether the private bit is set.
 */
bool AccessFlags::isPrivate() const {
	return accessFlags & ACC_PRIVATE;
}

/**
 * Returns whether the protected bit is set.
 */
bool AccessFlags::isProtected() const {
	return accessFlags & ACC_PROTECTED;
}

/**
 * Returns whether the static bit is set.
 */
bool AccessFlags::isStatic() const {
	return accessFlags & ACC_STATIC;
}

/**
 * Returns whether the "super" bit is set. This is for classes. This implies:
 * Treat superclass methods specially when invoked by the invokespecial instruction.
 * The ACC_SUPER flag indicates which of two alternative semantics is to be expressed by the invokespecial 
 * instruction (§invokespecial) if it appears in this class. Compilers to the instruction set of the Java virtual machine should set the ACC_SUPER flag.
 */
bool AccessFlags::isSuper() const {
	return accessFlags & ACC_SUPER;
}

/**
 * Returns whether the final bit is set.
 */
bool AccessFlags::isFinal() const {
	return accessFlags & ACC_FINAL;
}

/**
 * Returns whether the synchronized bit is set. Is only for methods.
 */
bool AccessFlags::isSynchronized() const {
	return accessFlags & ACC_SYNCHRONIZED;
}

/**
 * Returns whether the bridge bit is set. Is only for methods.
 */
bool AccessFlags::isBridge() const {
	return accessFlags & ACC_BRIDGE;
}

/** 
 * Returns whether the volatile bit is set. Is only for fields.
 */
bool AccessFlags::isVolatile() const {
	return accessFlags & ACC_VOLATILE;
}

/**
 * Returns whether the varargs bit is set. Is only for methods.
 */
bool AccessFlags::isVarArgs() const {
	return accessFlags & ACC_VARARGS;
}

/**
 * Returns whether the transient bit is set. Is only for fields.
 */
bool AccessFlags::isTransient() const {
	return accessFlags & ACC_TRANSIENT;
}

/**
 * Returns whether the native bit is set. Is only for methods.
 */
bool AccessFlags::isNative() const {
	return accessFlags & ACC_NATIVE;
}

/**
 * Returns whether the "interface" bit is set. Is only for classes.
 * When this is set, this class is an interface.
 * If the ACC_INTERFACE flag of this class file is set, its ACC_ABSTRACT flag must also be set (JLS §9.1.1.1). 
 * Such a class file must not have its ACC_FINAL, ACC_SUPER or ACC_ENUM flags set.
 */
bool AccessFlags::isInterface() const {
	return accessFlags & ACC_INTERFACE;
}

/**
 * Returns whether the abstract bit is set. Is only for methods.
 */
bool AccessFlags::isAbstract() const {
	return accessFlags & ACC_ABSTRACT;
}

/**
 * Returns whether the strict bit is set. Is only for methods.
 */
bool AccessFlags::isStrict() const {
	return accessFlags & ACC_STRICT;
}

/**
 * Returns whether the synthetic bit is set.
 */
bool AccessFlags::isSynthetic() const {
	return accessFlags & ACC_SYNTHETIC;
}

/**
 * Returns whether the enum bit is set. Is only for fields
 */
bool AccessFlags::isEnum() const {
	return accessFlags & ACC_ENUM;
}

/**
 * Gets the entire bit vector of the access flags.
 */
uint16_t AccessFlags::getValue() const {
	return accessFlags;
}

/**
 * Constructs a ClassMember out of a refernce to its ClassFile and an input
 * stream to read the data from.
 */
ClassMember::ClassMember(ClassFile& cf,std::istream& in) try :
	cf(cf),
	accessFlags(in),
	nameIndex(readShortUnsigned(in)),
	descriptorIndex(readShortUnsigned(in)),
	attributes(cf, in) {
	
} catch(...) {
	throw;
}

/**
 * Destructor for the ClassMember. Nothing is directly allocated by it, so it
 * doesn't delete anything.
 */
ClassMember::~ClassMember() {
	
}

/**
 * Gets the object associated with the accessibility flags for this field or method.
 */
const AccessFlags& ClassMember::getAccessFlags() const {
	return accessFlags;
}

/**
 * Gets the index in the constant table for the ConstantUtf8 name of this field or method.
 */
uint16_t ClassMember::getNameIndex() const {
	return nameIndex;
}

/**
 * Gets the index in the constant table for the ConstantUtf8 descriptor of this field or method.
 */
uint16_t ClassMember::getDescriptorIndex() const {
	return descriptorIndex;
}

/**
 * Gets the attribute pool that is a part of this method or field.
 */
const AttributePool& ClassMember::getAttributes() const {
	return attributes;
}

/**
 * Gets the utf8 string with the name of this field.
 */
const ustring& ClassMember::getName() const {
	return cf.getConstantPool().get<const ConstantUtf8&>(nameIndex).getStringValue();
}

/**
 * Gets the utf8 string with the descriptor of this field.
 */
const ustring& ClassMember::getDescriptor() const {
	return cf.getConstantPool().get<const ConstantUtf8&>(descriptorIndex).getStringValue();
}

/**
 * Constructor for the ClassMemberPool that takes in the associated ClassFile, and an input stream to
 * read the data from. It finds out from the input stream how many members there are, and reads them
 * all in.
 */
ClassMemberPool::ClassMemberPool(ClassFile& cf, istream& in) try : cf(cf), members(readShortUnsigned(in)) {
	uint16_t i;
	try {
		for(i = 0; i < members.size(); i++) {
			members[i] = new ClassMember(cf, in);
		}
	} catch(...) {
		for(uint16_t j = 0; j < i; j++) {
			delete members[i];
		}
		throw;
	}
} catch(...) {
	throw;
}

/**
 * Destructor for ClassMemberPool. Deletes all the allocated ClassMembers.
 */
ClassMemberPool::~ClassMemberPool() {
	for(uint16_t i = 0; i < members.size(); i++) {
		delete members[i];
	}
}

/**
 * Gets the number of members held by this ClassMemberPool.
 */
uint16_t ClassMemberPool::numMembers() const {
	return members.size();
}

/**
 * Gets a specific class member.
 */
ClassMember& ClassMemberPool::operator[](uint16_t index) {
	if(index >= members.size()) {
		throw runtime_error("Class member index out of range: " + toString(index) + " >= " + toString(members.size()));
	}
	return *(members[index]);
}

/**
 * Gets a specific const class member.
 */
const ClassMember& ClassMemberPool::operator[](uint16_t index) const {
	if(index >= members.size()) {
		throw runtime_error("Class member index out of range: " + toString(index) + " >= " + toString(members.size()));
	}
	return *(members[index]);
}




