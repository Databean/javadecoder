#include "AttributePool.h"
#include "ClassFile.h"
#include "Util.h"

#include <set>
#include <stdexcept>

using std::istream;
using Glib::ustring;
using std::cout;
using std::endl;
using std::set;
using std::runtime_error;
using std::string;

/**
 * Constructs an AttributePool using the ClassFile that it's a part of, out of a stream.
 */
AttributePool::AttributePool(ClassFile& classFile, istream& input) try : 
	classFile(classFile), constantPool(classFile.getConstantPool()), attributes(readShortUnsigned(input), NULL) {
	
	uint16_t i;
	try {
		for(i = 0; i < attributes.size(); i++) {
			attributes[i] = buildAttribute(input);
		}
	}
	catch(...) {
		for(uint16_t j = 0; j < i; j++) {
			
		}
	}
} catch(...) {
	throw;
}

/**
 * Destructor for AttributePool. Deletes all the attributes that are a part of the pool.
 */
AttributePool::~AttributePool() {
	for(uint16_t i = 0; i < attributes.size(); i++) {
		delete attributes[i];
	}
}

/**
 * Builds an attribute by reading its name, and constructing the appropriate object for it.
 */
Attribute* AttributePool::buildAttribute(istream& input) {
	uint16_t nameIndex = readShortUnsigned(input);
	const ustring& name = constantPool.get<ConstantUtf8&>(nameIndex).getStringValue();
	//TODO: Insert conditionals for attribute names as their corresponding classes are created.
	//TODO: if i take out the string(...) part, i get valgrind errors all over the place. I don't know if this is glib or me.
	if(string(name) == string(ConstantValueAttribute::name)) {
		return new ConstantValueAttribute(*this, nameIndex, input);
	} else {
		return new UnknownAttribute(*this, nameIndex, input);
	}
}

/**
 * Returns the class file this Attribute Pool is associated with.
 */
ClassFile& AttributePool::getClassFile() {
	return classFile;
}

/**
 * Returns the class file this Attribute Pool is associated with.
 */
const ClassFile& AttributePool::getClassFile() const {
	return classFile;
}

/**
 * Returns the number of attributes present in this Attribute Pool.
 */
uint16_t AttributePool::getNumAttributes() const {
	return attributes.size();
}

/**
 * Returns whether there is an attribute with the given name in the Attribute Pool.
 */
bool AttributePool::containsAttribute(const ustring& name) const {
	for(uint16_t i = 0; i < attributes.size(); i++) {
		if(attributes[i] != NULL) {
			if(attributes[i]->getName() == name) {
				return true;
			}
		}
	}
	return false;
}

/**
 * Returns the attribute with the given name.
 * TODO: Verify assumption that a single attribute pool cannot contain two attributes with the same name.
 */
const Attribute& AttributePool::getAttribute(const ustring& name) const {
	for(uint16_t i = 0; i < attributes.size(); i++) {
		if(attributes[i] != NULL) {
			if(attributes[i]->getName() == name) {
				return *(attributes[i]);
			}
		}
	}
	throw runtime_error("No attribute present with name \"" + name + "\"");
	return *((Attribute*)NULL);
}

/**
 * Constructor for the Attribute superclass. Takes in the attribute pool, name index, and length.
 */
Attribute::Attribute(AttributePool& attributePool, uint16_t nameIndex, uint32_t length) :
	attributePool(attributePool),
	nameIndex(nameIndex),
	length(length) {
	
}

/**
 * Destructor for the Attribute superclass. Nothing is allocated by this class, so nothing is deleted.
 */
Attribute::~Attribute() {
	
}

/**
 * Gets the Attribute Pool that this attribute is a part of.
 */
AttributePool& Attribute::getAttributePool() {
	return attributePool;
}

/**
 * Gets the Attribute Pool that this attribute is a part of.
 */
const AttributePool& Attribute::getAttributePool() const {
	return attributePool;
}

/**
 * Gets the index in the constant pool of the ConstantUtf8 representing the name of this object.
 */
uint16_t Attribute::getNameIndex() const {
	return nameIndex;
}

/**
 * Gets the length of this attribute, in bytes.
 */
uint32_t Attribute::getLength() const {
	return length;
}

/**
 * Gets the UTF-8 string representing the name of this attribute.
 */
const ustring& Attribute::getName() const {
	return getAttributePool().getClassFile().getConstantPool().get<const ConstantUtf8&>(getNameIndex()).getStringValue();
}

/**
 * Constructs an attribute of unknown type, using the attribute pool, the name index, and an input stream.
 */
UnknownAttribute::UnknownAttribute(AttributePool& attributePool, uint16_t nameIndex, std::istream& input) : 
	Attribute(attributePool, nameIndex, readIntUnsigned(input)) {
	
	info = new uint8_t[getLength()];
	input.read((char*)info, getLength());
}

/**
 * Desturctor for an UnknownAttribute. Frees the memory associated with it.
 */
UnknownAttribute::~UnknownAttribute() {
	delete[] info;
}

/**
 * Getter for the data held by this UnknownAttribute, in case someone else can make sense of it.
 */
const uint8_t* UnknownAttribute::getInfo() const {
	return info;
}

const ustring ConstantValueAttribute::name = "ConstantValue";

/**
 * Constructs an attribute of unknown type, using the attribute pool, the name index, and an input stream.
 */
ConstantValueAttribute::ConstantValueAttribute(AttributePool& attributePool, uint16_t nameIndex, istream& input) :
	Attribute(attributePool, nameIndex, readIntUnsigned(input)) {
	
	index = readShortUnsigned(input);
}

/**
 * Destructor for the ConstantValueAttribute. Nothign is specially allocated by this, so it doesn't do anything.
 */
ConstantValueAttribute::~ConstantValueAttribute() {
	
}

/**
 * Gets the index of the constant associated with this attribute in the constant pool.
 */
uint16_t ConstantValueAttribute::getIndex() const {
	return index;
}


