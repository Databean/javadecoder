#ifndef CLASS_MEMBER_H
#define CLASS_MEMBER_H

#include "AttributePool.h"
#include "ConstantPool.h"
#include <iostream>

/**
 * This class wraps the access_flags attribute present in method and field structures in the class file. It has
 * convenience methods for checking all of the bits, so that the bitwise operations for checking visibility, access, et cetera
 * does not depend on explicitly doing bitwise operations.
 */
class AccessFlags {
private:
	uint16_t accessFlags;
public:
	AccessFlags(uint16_t value);
	AccessFlags(std::istream& in);
	AccessFlags(const AccessFlags& af);
	virtual ~AccessFlags();
	virtual const AccessFlags& operator=(const AccessFlags& af);
	
	bool isPublic() const;
	bool isPrivate() const;
	bool isProtected() const;
	bool isStatic() const;
	bool isFinal() const;
	bool isSuper() const;
	bool isSynchronized() const;
	bool isBridge() const;
	bool isVolatile() const;
	bool isVarArgs() const;
	bool isTransient() const;
	bool isNative() const;
	bool isInterface() const;
	bool isAbstract() const;
	bool isStrict() const;
	bool isSynthetic() const;
	bool isEnum() const;
	
	uint16_t getValue() const;
};

/**
 * This class represents a field or a method in the class file. They are combined into one class here
 * since they have the exact same structure.
 */
class ClassMember {
private:
	ClassFile& cf;
	AccessFlags accessFlags;
	uint16_t nameIndex;
	uint16_t descriptorIndex;
	AttributePool attributes;
	
	ClassMember(ClassMember& cm) : cf(cm.cf), accessFlags(0), attributes(cm.cf, *((std::istream*)NULL)) {} //You really don't want to call this one.
	virtual ClassMember& operator=(const ClassMember &cm) { return *this; }
public:
	ClassMember(ClassFile& cf,std::istream& in);
	virtual ~ClassMember();
	
	const AccessFlags& getAccessFlags() const;
	uint16_t getNameIndex() const;
	uint16_t getDescriptorIndex() const;
	const AttributePool& getAttributes() const;
	
	const Glib::ustring& getName() const;
	const Glib::ustring& getDescriptor() const;
};

/**
 * This represents all of either the fields or the methods in a class file. This class manages the fact that
 * a certain number have to be read, and moves that issue out of the ClassFile class.
 */
class ClassMemberPool {
private:
	ClassFile& cf;
	std::vector<ClassMember*> members;
	
public:
	ClassMemberPool(ClassFile& cf, std::istream& in);
	virtual ~ClassMemberPool();
	
	uint16_t numMembers() const;
	
	ClassMember& operator[](uint16_t index);
	const ClassMember& operator[](uint16_t index) const;
};

#endif