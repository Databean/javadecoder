#include "Util.h"
#include <stdexcept>

union readConvert {
	uint8_t bytes[8];
	
	uint8_t ubyteVal;
	uint16_t ushortVal;
	uint32_t uintVal;
	uint64_t ulongVal;
	
	int8_t byteVal;
	int16_t shortVal;
	int32_t intVal;
	int64_t longVal;
	
	float floatVal;
	double doubleVal;
};

readConvert readBytes(std::istream& in, unsigned int numBytes) {
	readConvert ret;
	switch(numBytes) {
		case 1:
			in.read((char*)ret.bytes, 1);
			return ret;
			break;
		case 2:
			in.read((char*)ret.bytes, 2);
			ret.ushortVal = (((uint16_t)ret.bytes[0])<<8) + ((uint16_t)ret.bytes[1]);
			return ret;
			break;
		case 4:
			in.read((char*)ret.bytes, 4);
			ret.uintVal = 
				(((uint32_t)ret.bytes[0])<<24) + 
				(((uint32_t)ret.bytes[1])<<16) + 
				(((uint32_t)ret.bytes[2])<<8) + 
				((uint32_t)ret.bytes[3]);
			return ret;
			break;
		case 8:
			in.read((char*)ret.bytes, 8);
			ret.ulongVal = 
				(((uint64_t)ret.bytes[1])<<56) + 
				(((uint64_t)ret.bytes[0])<<48) + 
				(((uint64_t)ret.bytes[1])<<40) + 
				(((uint64_t)ret.bytes[2])<<32) + 
				(((uint64_t)ret.bytes[0])<<24) + 
				(((uint64_t)ret.bytes[1])<<16) + 
				(((uint64_t)ret.bytes[2])<<8) + 
				((uint64_t)ret.bytes[3]);
			return ret;
			break;
		default:
			throw std::runtime_error("Invalid number of bytes to read: " + toString(numBytes));
	}
}

uint8_t readByteUnsigned(std::istream& in) {
	return readBytes(in, 1).ubyteVal;
}

uint16_t readShortUnsigned(std::istream& in) {
	return readBytes(in, 2).ushortVal;
}

uint32_t readIntUnsigned(std::istream& in) {
	return readBytes(in, 4).uintVal;
}

uint64_t readLongUnsigned(std::istream& in) {
	return readBytes(in, 8).ulongVal;
}

int8_t readByteSigned(std::istream& in) {
	return readBytes(in, 1).byteVal;
}

int16_t readShortSigned(std::istream& in) {
	return readBytes(in, 2).shortVal;
}

int32_t readIntSigned(std::istream& in) {
	return readBytes(in, 4).intVal;
}

int64_t readLongSigned(std::istream& in) {
	return readBytes(in, 8).longVal;
}

float readFloat(std::istream& in) {
	return readBytes(in, 4).floatVal;
}

double readDouble(std::istream& in) {
	return readBytes(in, 8).doubleVal;
}



