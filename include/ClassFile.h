#ifndef CLASS_FILE
#define CLASS_FILE

#include <fstream>
#include <vector>
#include <inttypes.h>
#include <glibmm/ustring.h>
#include "VirtualMachine.h"
#include "ClassInstance.h"
#include "ConstantPool.h"
#include "AttributePool.h"
#include "ClassMember.h"

class Frame;

class VirtualMachine;

/**
 * The abstraction representing a single .class file. This is initialized from an input stream, and
 * the VirtualMachine that the class is a part of.
 * TODO: Validation that everything has reasonable values.
 */
class ClassFile {
public:
	ClassFile(VirtualMachine& vm,std::istream& f);
	virtual ~ClassFile();
	
	virtual void initialize();
	
	uint32_t getMagic() const;
	uint16_t getMinorVersion() const;
	uint16_t getMajorVersion() const;
	
	ConstantPool& getConstantPool();
	const ConstantPool& getConstantPool() const;
	
	AccessFlags& getAccessFlags();
	const AccessFlags& getAccessFlags() const;
	
	ClassMemberPool& getFields();
	const ClassMemberPool& getFields() const;
	
	ClassMemberPool& getMethods();
	const ClassMemberPool& getMethods() const;
	
	AttributePool& getAttributes();
	const AttributePool& getAttributes() const;
	
private:
	//ClassFile(const ClassFile&) {}
	//const ClassFile& operator=(const ClassFile&) { return *this; }
	
	std::vector<uint16_t> buildInterfaces(std::istream& in);
	
	VirtualMachine& vm;
	
	ClassMember* clinit;
	
	uint32_t magic;
	uint16_t minor_version;
	uint16_t major_version;
	ConstantPool constantPool;
	AccessFlags access_flags;
	uint16_t this_class;
	uint16_t super_class;
	std::vector<uint16_t> interfaces;
	ClassMemberPool fields;
	ClassMemberPool methods;
	AttributePool attributePool;
};

#endif