#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <sstream>
#include <fstream>
#include <stdint.h>

/**
 * Converts any object that has a method to output it in a stream into a string. Classes willing to
 * be converted into strings should implement the following method globally:
 * std::ostream& operator<<(std::ostream&, const T&);
 * That way they can be converted into string using this method, or outputted into streams, depending
 * on which is more convenient at the time, without having to have separate methods.
 */
template<class T>
std::string toString(const T& a) {
	std::stringstream s;
	s << a;
	return s.str();
}

template<class T>
std::string toHexString(const T& a) {
	std::stringstream s;
	s << std::hex << a << std::dec;
	return s.str();
}

uint8_t readByteUnsigned(std::istream& in);
uint16_t readShortUnsigned(std::istream& in);
uint32_t readIntUnsigned(std::istream& in);
uint64_t readLongUnsigned(std::istream& in);

int8_t readByteSigned(std::istream& in);
int16_t readShortSigned(std::istream& in);
int32_t readIntSigned(std::istream& in);
int64_t readLongSigned(std::istream& in);

float readFloat(std::istream& in);
double readDouble(std::istream& in);

template<class T>
uint32_t lowBits(const T& v) {
	uint64_t val = reinterpret_cast<const uint64_t&>(v);
	return (val << ((sizeof(val)*8)/2)) >> ((sizeof(val)*8)/2);
}

template<class T>
uint32_t highBits(const T& v) {
	uint64_t val = reinterpret_cast<const uint64_t&>(v);
	return val >> ((sizeof(val)*8)/2); // times 8 bits per byte, divided by 2 for top half
}

#endif