#ifndef CONSTANT_POOL_H
#define CONSTANT_POOL_H
/**
 * 
 */

#include <stdint.h>
#include <iostream>
#include <vector>
#include <glibmm/ustring.h>

#include "Constants.h"

class Constant;
class ClassFile;
class ConstantNameAndType;

/**
 * Represents the cp_info array present in the class file. This holds all the constants used
 * in the class file, and manages them completely. The constant file has indexes from
 * 1 to numElements, inclusive.
 */
class ConstantPool {
private:
	ClassFile& cf;
	std::vector<Constant*> constants;
	
	Constant* readConstant(std::istream& in);
public:
	ConstantPool(ClassFile& cf, uint16_t numElements);
	ConstantPool(ClassFile& cf, std::istream& in);
	virtual ~ConstantPool();
	
	virtual Constant& operator[](uint16_t index);
	virtual const Constant& operator[](uint16_t index) const;
	
	/**
	 * This works like operator[], except it performs an additional cast.
	 */
	template<class T>
	T& get(uint16_t index) {
		return dynamic_cast<T&>((*this)[index]);
	}
	
	/**
	 * This works like operator[] const, except it performs an additional cast.
	 */
	template<class T>
	const T& get(uint16_t index) const {
		return dynamic_cast<const T&>((*this)[index]);
	}
	
	/**
	 * Determines whether the constant at index i is of a specific type.
	 */
	template<class T>
	bool isType(uint16_t index) const {
		return dynamic_cast<const T*>(&(*this)[index]) != NULL;
	}
	
	virtual ClassFile& getClass();
	virtual const ClassFile& getClass() const;
	
	virtual uint16_t getNumElements() const;
	
	virtual bool validate() const;
};

/**
 * Superclass for the various types of constants present in the cp_info array. Retrieve these
 * by interacting with the ConstantPool.
 */
class Constant {
private:
	uint8_t type;
	ConstantPool& pool;
	
	Constant(Constant& p) : pool(p.pool) {}
	virtual const Constant& operator=(const Constant&) { return *this; }
public:
	Constant(ConstantPool& pool, uint8_t type);
	virtual ~Constant();
	
	virtual ConstantPool& getConstantPool();
	virtual const ConstantPool& getConstantPool() const;
	
	virtual uint8_t getType() const;
	
	virtual bool validate() const = 0;
};

/**
 * Represents a ClassInfo type of constant. These exist so that MethodInfo and such can refer to
 * a class. This just keeps track of the class's name.
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.1
 */
class ConstantClassInfo : public Constant {
private:
	uint16_t classNameIndex;
	
	ConstantClassInfo(ConstantClassInfo& c) : Constant(c.getConstantPool(), 0) {}
	virtual const ConstantClassInfo& operator=(const ConstantClassInfo&) { return *this; }
public:
	ConstantClassInfo(ConstantPool& pool, uint16_t classNameIndex);
	ConstantClassInfo(ConstantPool& pool, std::istream& in);
	virtual ~ConstantClassInfo();
	
	virtual uint16_t getClassNameIndex() const;
	virtual const Glib::ustring& getClassName() const;
	
	virtual bool validate() const;
};

/**
 * Represents a field, method, or interface method present in a class that is referred to in the class file. 
 * Contains indexes of a ConstantClassInfo representing the class it is a part of, and of a
 * ConstantNameAndTypeInfo representing the type. 
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.2
 */
class ConstantMemberReference : public Constant {
private:
	uint16_t classIndex;
	uint16_t nameAndTypeIndex;
	
	ConstantMemberReference(ConstantMemberReference& c) : Constant(c.getConstantPool(), 0) {}
	virtual const ConstantMemberReference& operator=(const ConstantMemberReference&) { return *this; }
public:
	ConstantMemberReference(ConstantPool& pool, uint8_t type, uint16_t classIndex, uint16_t nameAndTypeIndex);
	ConstantMemberReference(ConstantPool& pool, uint8_t type, std::istream& in);
	virtual ~ConstantMemberReference();
	
	virtual uint16_t getClassIndex() const;
	virtual uint16_t getNameAndTypeIndex() const;
	
	virtual const ConstantClassInfo& getClass() const;
	virtual const ConstantNameAndType& getNameAndType() const;
	
	virtual bool validate() const;
};

/**
 * Represents a constant object of the type String. Contains an index of a ConstantUtf8, which should have
 * the actual string stored in this object.
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.3
 */
class ConstantString : public Constant {
private:
	uint16_t stringIndex;
	
	ConstantString(ConstantString& s) : Constant(s.getConstantPool(), 0) {}
	virtual const ConstantString& operator=(const ConstantString&) { return *this; }
public:
	ConstantString(ConstantPool& pool, uint16_t stringIndex);
	ConstantString(ConstantPool& pool, std::istream& in);
	virtual ~ConstantString();
	
	virtual uint16_t getStringIndex() const;
	
	virtual const Glib::ustring& getStringValue() const;
	
	virtual bool validate() const;
};

/**
 * Represents an integer constant in the constant pool. The integer is signed.
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.4
 */
class ConstantInteger : public Constant {
private:
	int32_t intValue;
	
	ConstantInteger(ConstantInteger& i) : Constant(i.getConstantPool(), 0) {}
	virtual const ConstantInteger& operator=(const ConstantInteger&) { return *this; }
public:
	ConstantInteger(ConstantPool& pool, int32_t intValue);
	ConstantInteger(ConstantPool& pool, std::istream& in);
	virtual ~ConstantInteger();
	
	virtual int32_t getIntValue() const;
	
	virtual bool validate() const;
};

/**
 * Represents a float constant in the constant pool.
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.4
 */
class ConstantFloat : public Constant {
private:
	float floatValue;
	
	ConstantFloat(ConstantFloat& f) : Constant(f.getConstantPool(), 0) {}
	virtual const ConstantFloat& operator=(const ConstantFloat&) { return *this; }
public:
	ConstantFloat(ConstantPool& pool, float floatValue);
	ConstantFloat(ConstantPool& pool, std::istream& in);
	virtual ~ConstantFloat();
	
	virtual float getFloatValue() const;
	
	virtual bool validate() const;
};

/**
 * Represents a 64-bit long constant in the constant pool. This occupies two indexes, but
 * referring to the second one is invalid.
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.5
 */
class ConstantLong : public Constant {
private:
	int64_t longValue;
	
	ConstantLong(ConstantLong& l) : Constant(l.getConstantPool(), 0) {}
	virtual const ConstantLong& operator=(const ConstantLong&) { return *this; }
public:
	ConstantLong(ConstantPool& pool, int64_t longValue);
	ConstantLong(ConstantPool& pool, std::istream& in);
	virtual ~ConstantLong();
	
	virtual int64_t getLongValue() const;
	virtual uint32_t getHighBits() const;
	virtual uint32_t getLowBits() const;
	
	virtual bool validate() const;
};

/**
 * Represents a double constant in the constant pool. This occupies two indexes, but
 * referring to the second one is invalid.
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.5
 */
class ConstantDouble : public Constant {
private:
	double doubleValue;
	
	ConstantDouble(ConstantDouble& d) : Constant(d.getConstantPool(), 0) {}
	virtual const ConstantDouble& operator=(const ConstantDouble&) { return *this; }
public:
	ConstantDouble(ConstantPool& pool, double doubleValue);
	ConstantDouble(ConstantPool& pool, std::istream& in);
	virtual ~ConstantDouble();
	
	virtual double getDoubleValue() const;
	virtual uint32_t getHighBits() const;
	virtual uint32_t getLowBits() const;
	
	virtual bool validate() const;
};

/**
 * Represents a NameAndType index in the constant pool. This is the name and type of either a method,
 * an interface method, or a field. It has indices for a ConstantUtf8 representing the name, and a
 * ConstantUtf8 representing either the field type or the method type.
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.6
 */
class ConstantNameAndType : public Constant {
private:
	uint16_t nameIndex;
	uint16_t descriptorIndex;
	
	ConstantNameAndType(ConstantNameAndType& nt) : Constant(nt.getConstantPool(), 0) {}
	virtual const ConstantNameAndType& operator=(const ConstantNameAndType&) { return *this; }
public:
	ConstantNameAndType(ConstantPool& pool, uint16_t nameIndex, uint16_t descriptorIndex);
	ConstantNameAndType(ConstantPool& pool, std::istream& in);
	virtual ~ConstantNameAndType();
	
	virtual uint16_t getNameIndex() const;
	virtual uint16_t getDescriptorIndex() const;
	
	virtual const Glib::ustring& getName() const;
	virtual const Glib::ustring& getTypeString() const;
	
	virtual bool validate() const;
};

/**
 * Represents an actual string in the file. These are stored in UTF-8 format in the class file,
 * so we use a Glib::ustring to represent them here.
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.7
 */
class ConstantUtf8 : public Constant {
private:
	uint16_t numBytes;
	Glib::ustring stringValue;
	
	ConstantUtf8(ConstantUtf8& u) : Constant(u.getConstantPool(), 0) {}
	virtual const ConstantUtf8& operator=(const ConstantUtf8&) { return *this; }
public:
	ConstantUtf8(ConstantPool& pool, uint16_t numBytes, const Glib::ustring& value);
	ConstantUtf8(ConstantPool& pool, std::istream& in);
	virtual ~ConstantUtf8();
	
	virtual uint16_t getNumBytes() const;
	virtual const Glib::ustring& getStringValue() const;
	
	virtual bool validate() const;
};

/**
 * TODO: figure out what this is even supposed to do.
 * Dear future reader: I don't know what this is supposed to do either.
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.8
 */
class ConstantMethodHandle : public Constant {
private:
	uint8_t referenceKind;
	uint16_t referenceIndex;
	
	ConstantMethodHandle(ConstantMethodHandle& mh) : Constant(mh.getConstantPool(), 0) {}
	virtual const ConstantMethodHandle& operator=(const ConstantMethodHandle&) { return *this; }
public:
	ConstantMethodHandle(ConstantPool& pool, uint8_t referenceKind, uint16_t referenceIndex);
	ConstantMethodHandle(ConstantPool& pool, std::istream& in);
	virtual ~ConstantMethodHandle();
	
	virtual uint8_t getReferenceKind() const;
	virtual uint16_t getReferenceIndex() const;
	
	virtual bool validate() const;
};

/**
 * This class represents a method type, without a name.
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.9
 */
class ConstantMethodType : public Constant {
private:
	uint16_t descriptorIndex;
	
	ConstantMethodType(ConstantMethodType& mt) : Constant(mt.getConstantPool(), 0) {}
	const ConstantMethodType& operator=(const ConstantMethodType&) { return *this; }
public:
	ConstantMethodType(ConstantPool& pool, uint16_t descriptorIndex);
	ConstantMethodType(ConstantPool& pool, std::istream& in);
	virtual ~ConstantMethodType();
	
	virtual uint16_t getDescriptorIndex() const;
	
	virtual const Glib::ustring& getDescriptor() const;
	
	virtual bool validate() const;
};

/**
 * This class represents metadata used with the new "invokedynamic" instruction, which was created
 * to support languages with runtime typing.
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.10
 */
class ConstantInvokeDynamic : public Constant {
private:
	uint16_t bootstrapMethodAttributeIndex;
	uint16_t nameAndTypeIndex;
	
	ConstantInvokeDynamic(ConstantInvokeDynamic& id) : Constant(id.getConstantPool(), 0) {}
	const ConstantInvokeDynamic& operator=(const ConstantInvokeDynamic&) { return *this; }
public:
	ConstantInvokeDynamic(ConstantPool& pool, uint16_t bootstrapMethodAttributeIndex, uint16_t nameAndTypeIndex);
	ConstantInvokeDynamic(ConstantPool& pool, std::istream& in);
	virtual ~ConstantInvokeDynamic();
	
	virtual uint16_t getBootstrapMethodAttributeIndex() const;
	virtual uint16_t getNameAndTypeIndex() const;
	
	virtual const ConstantNameAndType& getNameAndType() const;
	
	virtual bool validate() const;
};

#endif