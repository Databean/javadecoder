#ifndef FRAME_H
#define FRAME_H

#include <list>
#include "ClassFile.h"

struct returnInfo {
	u1 opcode;
	union {
		int32_t intValue;
		int64_t longValue;
		uint32_t refValue;
		float floatValue;
		double doubleValue;
		uint16_t index;
	};
};

class Frame {
private:
	VirtualMachine& vm;
	ClassMember& method;
	u1* code;
	ConstantPool& constants;
	u4 location;
	std::list<u4> stack;
	std::vector<u4> localVariables;
	
	inline void pushInt(const int32_t& i) { stack.push_back(i); }
	inline void pushShort(const int16_t& i) { stack.push_back(i); }
	inline void pushByte(const int8_t& i) { stack.push_back(i); }
	inline void pushBoolean(const bool& b) { stack.push_back(b); }
	inline void pushFloat(const float& f) { int32_t i = reinterpret_cast<const int32_t&>(f); stack.push_back(i); }
	inline void pushLong(const int64_t& l) { stack.push_back(l>>32); stack.push_back(l); }
	inline void pushDouble(const double& d) { pushLong(reinterpret_cast<const int64_t&>(d)); }
	
	void ldc(u2 index);
	
	void load(u1 index) { pushInt(localVariables[index]); }
	void load2(u1 index) { pushInt(localVariables[index]); pushInt(localVariables[index+1]); }
	
	void store(u1 index) { localVariables[index] = popInt(); }
	void store2(u1 index) { localVariables[index] = popInt(); localVariables[index+1] = popInt(); }
public:
	Frame(VirtualMachine& vm, ClassMember& method, ConstantPool& constants);
	virtual ~Frame();
	
	inline int32_t popInt() { int32_t ret = stack.back(); stack.pop_back(); return ret; }
	inline int16_t popShort() { int16_t ret = stack.back(); stack.pop_back(); return ret; }
	inline int8_t popByte() { int8_t ret = stack.back(); stack.pop_back(); return ret; }
	inline bool popBoolean() { bool ret = stack.back(); stack.pop_back(); return ret; }
	inline float popFloat() { float ret = reinterpret_cast<float&>(stack.back()); stack.pop_back(); return ret; }
	
	inline int64_t popLong() { 
		int64_t ret = reinterpret_cast<int64_t&>(stack.back()) << 32; stack.pop_back();
		ret += reinterpret_cast<int64_t&>(stack.back()); stack.pop_back();
		return ret;
	}
	inline double popDouble() { int64_t lng = popLong(); return reinterpret_cast<double&>(lng); }
	
	void run();
	
	//inline int32_t pop() { int32_t ret = stack.back(); stack.pop_back(); return ret; }
	//inline void push(const int32_t& a) { stack.push_back(a); }
};

/*
template<class T> inline T framePop(Frame* f) { return reinterpret_cast<T&>(f->pop()); }
template<> inline int64_t framePop<int64_t>(Frame* f) { return (framePop<int64_t>(f) << 32) + framePop<int64_t>(f); }
template<> inline double framePop<double>(Frame* f) { int64_t i = framePop<int64_t>(f); return reinterpret_cast<double&>(i); }

template<class T> inline void framePush(Frame* frame,const T& a) { frame->push(a); }
template<> inline void framePush<int64_t>(Frame* frame,const int64_t& a) { frame->push(a>>32); frame->push(a); }
*/

#endif