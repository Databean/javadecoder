#include "ConstantPool.h"
#include <stdlib.h>
#include <stdexcept>
#include "Util.h"

using std::istream;
using std::vector;
using Glib::ustring;

/** 
 * Constructs a constant pool to hold a given number of elements. Also keeps track of what class file the
 * constant pool belongs to.
 */
ConstantPool::ConstantPool(ClassFile& cf, uint16_t numElements) try : cf(cf), constants(numElements, NULL) {
	
} catch(...) {
	throw;
}

/**
 * Constructs a constant pool by reading in the appropriate section from an input stream. It is assumed
 * that the input stream begins at the beginning of the constant pool size, which is "constant_pool_count".
 */
ConstantPool::ConstantPool(ClassFile& cf, istream& in) try : cf(cf), constants(readShortUnsigned(in) - 1, NULL) {
	uint16_t i;
	try {
		for(i = 0; i < constants.size(); i++) {
			constants[i] = readConstant(in);
			if(constants[i]->getType() == CONSTANT_Double || constants[i]->getType() == CONSTANT_Long) {
				i++;
				constants[i] = NULL;
			}
		}
	} catch(...) { //If there was a read error part-way through, we need to delete the objects for the already constructed constants.
		for(uint16_t j = 0; j < i; j++) {
			if(constants[j] != NULL) {
				delete constants[j];
			}
		}
		throw;
	}
} catch(...) {
	throw;
}

/**
 * Destroys the constant pool, deleting all of its associated constants.
 */
ConstantPool::~ConstantPool() {
	for(vector<Constant*>::iterator it = constants.begin(); it != constants.end(); it++) {
		// Double and Long constants count for two indexes, with the second one being NULL.
		if((*it) != NULL) {
			delete (*it);
		}
	}
}

/**
 * Reads data in from a stream, and constructs the appropriate constant for it.
 */
Constant* ConstantPool::readConstant(istream& in) {
	uint8_t constantType = readByteUnsigned(in);
	switch(constantType) {
		case CONSTANT_Class:
			return new ConstantClassInfo(*this, in);
		case CONSTANT_Fieldref:
		case CONSTANT_Methodref:
		case CONSTANT_InterfaceMethodref:
			return new ConstantMemberReference(*this, constantType, in);
		case CONSTANT_String:
			return new ConstantString(*this, in);
		case CONSTANT_Integer:
			return new ConstantInteger(*this, in);
		case CONSTANT_Float:
			return new ConstantFloat(*this, in);
		case CONSTANT_Long:
			return new ConstantLong(*this, in);
		case CONSTANT_Double:
			return new ConstantDouble(*this, in);
		case CONSTANT_NameAndType:
			return new ConstantNameAndType(*this, in);
		case CONSTANT_Utf8:
			return new ConstantUtf8(*this, in);
		case CONSTANT_MethodHandle:
			return new ConstantMethodHandle(*this, in);
		case CONSTANT_MethodType:
			return new ConstantMethodType(*this, in);
		case CONSTANT_InvokeDynamic:
			return new ConstantInvokeDynamic(*this, in);
		default:
			throw "Constant type not known: " + toString<int>(constantType);
	};
}

/**
 * Retrieves the constant at the specific index. Valid constant indexes are in the range
 * [1, numElements], inclusive. Going out of this range will throw an exception.
 */
Constant& ConstantPool::operator[](uint16_t index) {
	if(index == 0 || index > constants.size()) {
		throw std::runtime_error("Constant " + toString(index) + " is out of range [1, " + toString(constants.size()) + "].");
	}
	return *(constants[index-1]);
}

/**
 * Retrieves the const constant at the specific index. Valid constant indexes are in the
 * range [1, numElements], inclusive. Going out of this range will throw an exception.
 */
const Constant& ConstantPool::operator[](uint16_t index) const {
	if(index == 0 || index > constants.size()) {
		throw std::runtime_error("Constant " + toString(index) + " is out of range [1, " + toString(constants.size()) + "].");
	}
	return *(constants[index-1]);
}

ClassFile& ConstantPool::getClass() {
	return cf;
}

const ClassFile& ConstantPool::getClass() const {
	return cf;
}

uint16_t ConstantPool::getNumElements() const {
	return constants.size();
}

bool ConstantPool::validate() const {
	for(uint16_t i = 0; i < constants.size(); i++) {
		if(constants[i] != NULL && !constants[i]->validate()) {
			return false;
		}
	}
	return true;
}

/**
 * Superclass constructor for Constant. Stores a reference to the constant pool, as well
 * as the type number associated with this sort of constant.
 */
Constant::Constant(ConstantPool& pool, uint8_t type) : type(type), pool(pool) {
	
}

/**
 * Superclass destructor for Constant.
 */
Constant::~Constant() {
	
}

/**
 * Gets the constant pool that this constant is a part of.
 */
ConstantPool& Constant::getConstantPool() {
	return pool;
}

/**
 * Gets the constant pool that this constant is a part of.
 */
const ConstantPool& Constant::getConstantPool() const {
	return pool;
}

/**
 * Function to return the type of a particular constant. This will be one of the CONSTANT_ numbers in the
 * Constants.h file.
 */
uint8_t Constant::getType() const {
	return type;
}

/**
 * Constructor for the ConstantClassInfo. Takes in the index of the name of the class.
 */
ConstantClassInfo::ConstantClassInfo(ConstantPool& pool, uint16_t classNameIndex) :
	Constant(pool, CONSTANT_Class), 
	classNameIndex(classNameIndex) {
	
}

/**
 * Constructor for the ConstantClassInfo that takes in an input stream to read from.
 */
ConstantClassInfo::ConstantClassInfo(ConstantPool& pool, istream& in) :
	Constant(pool, CONSTANT_Class) {
	
	classNameIndex = readShortUnsigned(in);
}

/**
 * Destructor for the ConstantClassInfo. There is nothing allocated for this, so there is
 * nothing to delete.
 */
ConstantClassInfo::~ConstantClassInfo() {
	
}

/**
 * Gets the index of the class name. 
 */
uint16_t ConstantClassInfo::getClassNameIndex() const {
	return classNameIndex;
}

/**
 * Gets the actual class name.
 */
const ustring& ConstantClassInfo::getClassName() const {
	return getConstantPool().get<ConstantUtf8>(classNameIndex).getStringValue();
}

/**
 * Returns whether the ConstantClassInfo is valid, based on whether the classNameIndex is a ConstantUtf8.
 */
bool ConstantClassInfo::validate() const {
	return getConstantPool().isType<ConstantUtf8>(classNameIndex);
}

/**
 * Constructor for the member information in a class file. Takes in indexes for the class and the name and type. If the passed in type
 * is not a CONSTANT_Fieldref, CONSTANT_Methodref, or CONSTANT_InterfaceMethodref, an exception will be thrown.
 */
ConstantMemberReference::ConstantMemberReference(ConstantPool& pool, uint8_t type, uint16_t classIndex, uint16_t nameAndTypeIndex) :
	Constant(pool, type),
	classIndex(classIndex),
	nameAndTypeIndex(nameAndTypeIndex) {
	
	if(!(type == CONSTANT_Fieldref ||
		type == CONSTANT_Methodref ||
		type == CONSTANT_InterfaceMethodref)) {
		
		throw std::runtime_error("Not a valid type of ConstantMemberReference (" + toString(type) + ").");
	}
}

/**
 * Constructs a ConstantMemberReference, taking in a binary stream which contains the appropriate values.
 */
ConstantMemberReference::ConstantMemberReference(ConstantPool& pool, uint8_t type, istream& in) :
	Constant(pool, type) {
	
	classIndex = readShortUnsigned(in);
	nameAndTypeIndex = readShortUnsigned(in);
}

/**
 * Destructor for the ConstantMemberReference. It doesn't allocate anything, so no deletion is necessary.
 */
ConstantMemberReference::~ConstantMemberReference() {
	
}

/**
 * Gets the index for the class info for the class this class member belongs to.
 */
uint16_t ConstantMemberReference::getClassIndex() const {
	return classIndex;
}

/**
 * Gets the index for the name and type structure for the properties of this class member.
 */
uint16_t ConstantMemberReference::getNameAndTypeIndex() const {
	return nameAndTypeIndex;
}

/**
 * Returns the ConstantClassInfo this ConstantMemberReference is a part of.
 */
const ConstantClassInfo& ConstantMemberReference::getClass() const {
	return getConstantPool().get<ConstantClassInfo>(classIndex);
}

/**
 * Returns the ConstantNameAndType this ConstantMemberReference refers to.
 */
const ConstantNameAndType& ConstantMemberReference::getNameAndType() const {
	return getConstantPool().get<ConstantNameAndType>(nameAndTypeIndex);
}

/**
 * Returns whether this ConstantMemberReference is valid, that is, whether its stored indices for
 * ConstantClassInfo and ConstantNameAndType do indeed refer to Constants of those type in the pool.
 */
bool ConstantMemberReference::validate() const {
	const ConstantPool& pool = getConstantPool();
	return pool.isType<ConstantClassInfo>(classIndex) && pool.isType<ConstantNameAndType>(nameAndTypeIndex);
}

/**
 * Constructor for a constant string object in a class file. Takes in an index for a ConstantUtf8 which
 * holds the actual string.
 */
ConstantString::ConstantString(ConstantPool& pool, uint16_t stringIndex) :
	Constant(pool, CONSTANT_String),
	stringIndex(stringIndex) {
	
}

/**
 * Constructor for a ConstantString in a class file. Takes in a binary input stream where the data should be.
 */
ConstantString::ConstantString(ConstantPool&  pool, istream& in) :
	Constant(pool, CONSTANT_String) {
	
	stringIndex = readShortUnsigned(in);
}

/**
 * Destructor for ConstantString. Since nothing is allocated, nothing needs to be deleted.
 */
ConstantString::~ConstantString() {
	
}

/**
 * Gets the index of the associated ConstantUtf8.
 */
uint16_t ConstantString::getStringIndex() const {
	return stringIndex;
}

/**
 * Gets the string that this object refers to.
 */
const ustring& ConstantString::getStringValue() const {
	return getConstantPool().get<ConstantUtf8>(stringIndex).getStringValue();
}

/**
 * Makes sure the ConstantString is valid (it refers to a ConstantUtf8).
 */
bool ConstantString::validate() const {
	return getConstantPool().isType<const ConstantUtf8>(stringIndex);
}

/**
 * Constructor for a constant integer value in a class file. Takes in the actual value.
 */
ConstantInteger::ConstantInteger(ConstantPool& pool, int32_t value) :
	Constant(pool, CONSTANT_Integer),
	intValue(value) {
	
}

/**
 * Constructor for a constant integer value in a class file. Takes in a stream to read the data from.
 */
ConstantInteger::ConstantInteger(ConstantPool& pool, istream& in) :
	Constant(pool, CONSTANT_Integer) {
	
	intValue = readIntSigned(in);
}

/**
 * Destructor for ConstantInteger. Nothing is allocated for this, so nothing is deleted.
 */
ConstantInteger::~ConstantInteger() {
	
}

/**
 * Gets the integer value stored with this constant.
 */
int32_t ConstantInteger::getIntValue() const {
	return intValue;
}

/**
 * Returns whether the integer is valid. Integers are always valid.
 */
bool ConstantInteger::validate() const {
	return true;
}

/**
 * Constructor for a constant float value in a class file. Takes in the actual value.
 */
ConstantFloat::ConstantFloat(ConstantPool& pool, float value) :
	Constant(pool, CONSTANT_Float),
	floatValue(value) {
	
}

/**
 * Constructor for a constant float value in a class file. Takes in a stream to read the data from.
 */
ConstantFloat::ConstantFloat(ConstantPool& pool, istream& in) :
	Constant(pool, CONSTANT_Float) {
	
	floatValue = readFloat(in);
}

/**
 * Destructor for ConstantFloat. Nothing is allocated for this, so nothing is deleted.
 */
ConstantFloat::~ConstantFloat() {
	
}

/**
 * Gets the float value stored with this constant.
 */
float ConstantFloat::getFloatValue() const {
	return floatValue;
}

/**
 * Returns whether the float value is valid. Floats are always valid.
 */
bool ConstantFloat::validate() const {
	return true;
}

/**
 * Constructor for a constant signed 64-bit long in the class file. Takes in the actual value.
 */
ConstantLong::ConstantLong(ConstantPool& pool, int64_t value) :
	Constant(pool, CONSTANT_Long),
	longValue(value) {
	
}

/**
 * Constructor for a constant signed 64-bit long in the class file. Takes in a stream to read the data from.
 */
ConstantLong::ConstantLong(ConstantPool& pool, istream& in) :
	Constant(pool, CONSTANT_Long) {
	
	longValue = readLongSigned(in);
}

/**
 * Destructor for a constant long. Nothing is allocated by this, so nothing is deleted.
 */
ConstantLong::~ConstantLong() {
	
}

/**
 * Gets the 64-bit long value stored with this constant.
 */
int64_t ConstantLong::getLongValue() const {
	return longValue;
}

/**
 * Gets the 32 higher bits of the long constant.
 */
uint32_t ConstantLong::getHighBits() const {
	return highBits(longValue);
}

/**
 * Gets the 32 lower bits of the long constant.
 */
uint32_t ConstantLong::getLowBits() const {
	return lowBits(longValue);
}

/**
 * Returns whether the long value is valid. Longs are always valid.
 */
bool ConstantLong::validate() const {
	return true;
}

/**
 * Constructor for a double constant in the class file. Takes in the actual value.
 */
ConstantDouble::ConstantDouble(ConstantPool& pool, double value) :
	Constant(pool, CONSTANT_Double),
	doubleValue(value) {
	
}

/**
 * Constructor for a double constant in the class file. Takes in a binary stream to read the data from.
 */
ConstantDouble::ConstantDouble(ConstantPool& pool, istream& in) :
	Constant(pool, CONSTANT_Double) {
	
	doubleValue = readDouble(in);
}

/**
 * Destructor for a double constant. Nothing is allocated, so nothing is deleted.
 */
ConstantDouble::~ConstantDouble() {
	
}

/**
 * Gets the double stored with this constant.
 */
double ConstantDouble::getDoubleValue() const {
	return doubleValue;
}

/**
 * Gets the 32 higher bits of the double constant.
 */
uint32_t ConstantDouble::getHighBits() const {
	return highBits(doubleValue);
}

/**
 * Gets the 32 lower bits of the double constant.
 */
uint32_t ConstantDouble::getLowBits() const {
	return lowBits(doubleValue);
}

/**
 * Returns whether the double is valid. Doubles are always valid.
 */
bool ConstantDouble::validate() const {
	return true;
}

/**
 * Constructor for the ConstantNameAndType information in the class file. Takes in indices for the name
 * constant and the type descriptor constant, which are both ConstantUtf8s.
 */
ConstantNameAndType::ConstantNameAndType(ConstantPool& cp, uint16_t nameIndex, uint16_t descriptorIndex) :
	Constant(cp, CONSTANT_NameAndType),
	nameIndex(nameIndex),
	descriptorIndex(descriptorIndex) {
	
}

/**
 * Constructor for the ConstantNameAndType information in the class file. Takes in a stream to read the data from.
 */
ConstantNameAndType::ConstantNameAndType(ConstantPool& cp, istream& in) :
	Constant(cp, CONSTANT_NameAndType) {
	
	nameIndex = readShortUnsigned(in);
	descriptorIndex = readShortUnsigned(in);
}

/**
 * Destructor for the ConstantNameAndType object. Nothing is allocated, so there is nothing to be deleted.
 */
ConstantNameAndType::~ConstantNameAndType() {
	
}

/**
 * Gets the index for the ContantUtf8 object that holds the name associated with this constant.
 */
uint16_t ConstantNameAndType::getNameIndex() const {
	return nameIndex;
}

/**
 * Gets the index for the ConstantUtf8 object that holds the type information associated with this constant.
 */
uint16_t ConstantNameAndType::getDescriptorIndex() const {
	return descriptorIndex;
}

/**
 * Gets the name of this object, as a utf8 string.
 */
const ustring& ConstantNameAndType::getName() const {
	return getConstantPool().get<ConstantUtf8>(nameIndex).getStringValue();
}

/**
 * Gets the type for this object, as a utf8 string.
 */
const ustring& ConstantNameAndType::getTypeString() const {
	return getConstantPool().get<ConstantUtf8>(descriptorIndex).getStringValue();
}

/**
 * Returns whether the ConstantNameAndType is valid. It is valid if both the name and type are indexes of
 * ConstantUtf8s.
 */
bool ConstantNameAndType::validate() const {
	const ConstantPool& pool = getConstantPool();
	return pool.isType<const ConstantUtf8>(nameIndex) && pool.isType<const ConstantUtf8>(descriptorIndex);
}

/**
 * Constructor for the ConstantUtf8 object. Takes in the raw number of bytes, as well as the UTF-8 string.
 */
ConstantUtf8::ConstantUtf8(ConstantPool& pool, uint16_t numBytes, const ustring& stringValue) :
	Constant(pool, CONSTANT_Utf8),
	numBytes(numBytes),
	stringValue(stringValue) {
	
}

/**
 * Constructor for the ConstantUtf8 object. Takes in a binary stream to read the data from.
 */
ConstantUtf8::ConstantUtf8(ConstantPool& pool, istream& in) :
	Constant(pool, CONSTANT_Utf8) {
	
	numBytes = readShortUnsigned(in);
	char* data = new char[numBytes+1];
	try {
		in.read(data, numBytes);
		data[numBytes] = '\0';
		stringValue = ustring(data);
	} catch(...) {
		delete[] data;
		throw;
	}
	delete[] data;
}

/**
 * Destructor for the ConstantUtf8 object. Nothing is directly dynamically allocated, so there is no deletion.
 */
ConstantUtf8::~ConstantUtf8() {
	
}

/**
 * Returns the raw number of bytes occupied by the string.
 */
uint16_t ConstantUtf8::getNumBytes() const {
	return numBytes;
}

/**
 * Returns the string represented, in UTF-8 format.
 */
const ustring& ConstantUtf8::getStringValue() const {
	return stringValue;
}

/**
 * Returns whether the ConstantUtf8 is valid. ConstantUtf8s are, for now, always valid.
 * TODO: Check if the string is in valid utf8 or something?
 */
bool ConstantUtf8::validate() const {
	return true;
}

/**
 * Constructor for the MethodHandle constant. Takes in an integer describing which "kind" of MethodHandle it is,
 * and something whose meaning changes based on the kind.
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html#jvms-4.4.8
 */
ConstantMethodHandle::ConstantMethodHandle(ConstantPool& pool, uint8_t referenceKind, uint16_t referenceIndex) :
	Constant(pool, CONSTANT_MethodHandle),
	referenceKind(referenceKind),
	referenceIndex(referenceIndex) {
	
}

/**
 * Constructor for the MethodHandle constant. Takes in a binary stream to read the data from.
 */
ConstantMethodHandle::ConstantMethodHandle(ConstantPool& pool, istream& in) :
	Constant(pool, CONSTANT_MethodHandle) {
	
	referenceKind = readByteUnsigned(in);
	referenceIndex = readShortUnsigned(in);
}

/**
 * Destructor for the ConstantMethodHandle object. Nothing is allocated for this, so nothing is deleted.
 */
ConstantMethodHandle::~ConstantMethodHandle() {
	
}

/**
 * Returns the type of MethodHandle this is.
 */
uint8_t ConstantMethodHandle::getReferenceKind() const {
	return referenceKind;
}

/**
 * Returns the index of the method/field/something that this MethodHandle refers to.
 */
uint16_t ConstantMethodHandle::getReferenceIndex() const {
	return referenceIndex;
}

/**
 * TODO: Validate the ConstantMethodHandle
 */
bool ConstantMethodHandle::validate() const {
	return true;
}

/**
 * Constructs a ConstantMethodType, taking in the index of a ConstantUtf8 which is has a
 * string description of the type.
 */
ConstantMethodType::ConstantMethodType(ConstantPool& pool, uint16_t descriptorIndex) :
	Constant(pool, CONSTANT_MethodType),
	descriptorIndex(descriptorIndex) {
	
}

/**
 * Constructs a ConstantMethodType, taking in a binary stream to take the data from.
 */
ConstantMethodType::ConstantMethodType(ConstantPool& pool, istream& in) :
	Constant(pool, CONSTANT_MethodType) {
	
	descriptorIndex = readShortUnsigned(in);
}

/**
 * Destructor for ConstantMethodType. Nothing is allocated, so nothing is deleted here.
 */
ConstantMethodType::~ConstantMethodType() {
	
}

/**
 * Returns the index of the ConstantUtf8 which has a string description of the method type.
 */
uint16_t ConstantMethodType::getDescriptorIndex() const {
	return descriptorIndex;
}

/**
 * Returns the string description of the method type.
 */
const ustring& ConstantMethodType::getDescriptor() const {
	return getConstantPool().get<const ConstantUtf8&>(descriptorIndex).getStringValue();
}

/**
 * Returns whether the ConstantMethodType is valid, which is true if the descriptor index
 * is the index of a valid ConstantUtf8.
 */
bool ConstantMethodType::validate() const {
	return getConstantPool().isType<const ConstantUtf8>(descriptorIndex);
}

/**
 * Constructor for the ConstantInvokeDynamic object in the class file. Takes in an index for the relevant
 * bootstrap method in the class file's bootstrap method list, and the index for the name and type data
 * relevant for this method.
 */
ConstantInvokeDynamic::ConstantInvokeDynamic(ConstantPool& pool, uint16_t bootstrapMethodAttributeIndex, uint16_t nameAndTypeIndex) :
	Constant(pool, CONSTANT_InvokeDynamic),
	bootstrapMethodAttributeIndex(bootstrapMethodAttributeIndex),
	nameAndTypeIndex(nameAndTypeIndex) {
	
}

/**
 * Constructor for the ConstantInvokeDynamic object. Takes in a binary stream to read the data from.
 */
ConstantInvokeDynamic::ConstantInvokeDynamic(ConstantPool& pool, istream& in) :
	Constant(pool, CONSTANT_InvokeDynamic) {
	
	bootstrapMethodAttributeIndex = readShortUnsigned(in);
	nameAndTypeIndex = readShortUnsigned(in);
}

/**
 * Destructor for ConstantInvokeDynamic. Nothing is allocated for this, so nothing is deleted.
 */
ConstantInvokeDynamic::~ConstantInvokeDynamic() {
	
}

/**
 * Returns the index of the relevant bootstrap method in the class file's bootstrap method list.
 */
uint16_t ConstantInvokeDynamic::getBootstrapMethodAttributeIndex() const {
	return bootstrapMethodAttributeIndex;
}

/**
 * Returns the index of the name and type data relevant to this method.
 */
uint16_t ConstantInvokeDynamic::getNameAndTypeIndex() const {
	return nameAndTypeIndex;
}

/**
 * Returns the ConstantNameAndType data relevant to this method.
 */
const ConstantNameAndType& ConstantInvokeDynamic::getNameAndType() const {
	return getConstantPool().get<const ConstantNameAndType>(nameAndTypeIndex);
}

/**
 * Returns whether the ConstantInvokeDynamic is valid, which means both the bootstrap method index
 * and the ConstantNameAndType index are valid.
 * TODO: make this deal with the bootstrap method.
 */
bool ConstantInvokeDynamic::validate() const {
	return getConstantPool().isType<const ConstantNameAndType>(nameAndTypeIndex);
}


