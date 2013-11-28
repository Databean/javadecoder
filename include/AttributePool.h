#ifndef ATTRIBUTE_POOL
#define ATTRIBUTE_POOL

#include <iostream>
#include <vector>

#include <glibmm/ustring.h>

#include "ConstantPool.h"

class Attribute;

/**
 * While Java never refers to an "attribute pool", the attributes for the class file, fields, and methods all work the same,
 * so this class consolidates that functionality. It's built using a stream as input, and the binary format consists of
 * the number of attributes, followed by the list of attributes. Unlike constants, we don't actually have to know all the
 * types of attributes; the attributes all come with a size so we can skip over ones we don't understand (these will be of
 * type UnknownAttribute).
 */
class AttributePool {
private:
	ClassFile& classFile;
	ConstantPool& constantPool;
	std::vector<Attribute*> attributes;
	
	Attribute* buildAttribute(std::istream& input);
	
	AttributePool(AttributePool& p) : classFile(p.classFile), constantPool(p.constantPool) {}
	virtual const AttributePool& operator=(AttributePool& attributePool) { return *this; }
public:
	AttributePool(ClassFile& classFile, std::istream& input);
	virtual ~AttributePool();
	
	ClassFile& getClassFile();
	const ClassFile& getClassFile() const;
	
	uint16_t getNumAttributes() const;
	
	bool containsAttribute(const Glib::ustring& name) const;
	const Attribute& getAttribute(const Glib::ustring& name) const;
};

/**
 * Superclass for all attributes. These values are common to all attributes, so they're being put here.
 */
class Attribute {
private:
	AttributePool& attributePool;
	uint16_t nameIndex;
	uint32_t length;
	
	Attribute(Attribute& a) : attributePool(a.attributePool) {}
	virtual const Attribute& operator=(Attribute& a) { return *this; }
public:
	Attribute(AttributePool& attributePool, uint16_t nameIndex, uint32_t length);
	virtual ~Attribute();
	
	AttributePool& getAttributePool();
	const AttributePool& getAttributePool() const;
	
	uint16_t getNameIndex() const;
	uint32_t getLength() const;
	
	const Glib::ustring& getName() const;
};

/**
 * Class for attributes that we can't recognize and are therefore skipping over.
 */
class UnknownAttribute : public Attribute {
private:
	uint8_t* info;
	
	UnknownAttribute(UnknownAttribute& a) : Attribute(a.getAttributePool(), a.getNameIndex(), a.getLength()) {}
	virtual const UnknownAttribute& operator=(UnknownAttribute& a) { return *this; }
public:
	UnknownAttribute(AttributePool& attributePool, uint16_t nameIndex, std::istream& input);
	virtual ~UnknownAttribute();
	
	const uint8_t* getInfo() const;
};

/**
 * Class for the ConstantValue Attribute, which is used to assign an initial value to class fields, both static
 * and non-static. Each field_info can have at most one of these, and they should not be present elsewhere.
 */
class ConstantValueAttribute : public Attribute {
private:
	uint16_t index;
	
	ConstantValueAttribute(ConstantValueAttribute& a) : Attribute(a.getAttributePool(), a.getNameIndex(), a.getLength()) {}
	virtual const ConstantValueAttribute& operator=(ConstantValueAttribute& c) { return *this; }
public:
	const static Glib::ustring name;
	
	ConstantValueAttribute(AttributePool& attributePool, uint16_t nameIndex, std::istream& input);
	virtual ~ConstantValueAttribute();
	
	uint16_t getIndex() const;
};

#endif