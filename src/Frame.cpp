#include "Frame.h"
#include "Constants.h"
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "Util.h"

using namespace std;

Frame::Frame(VirtualMachine& vm, ClassMember& method, ConstantPool& constants) :
	vm(vm), method(method), constants(constants), location(0) {
	
}
Frame::~Frame() {
	
}
#define GET_U1() (code[location++])
#define GET_U2() (location+=2,(((u2)code[location-2])<<8) + ((u2)code[location-1]))
#define GET_U4() (location+=4,(((u4)code[location-4])<<24) + (((u2)code[location-3])<<16) + (((u2)code[location-2])<<8) + ((u2)code[location-1]))
void Frame::run() {
	u1 opcode;
	while(true) {
		opcode = GET_U1();
		switch(opcode) {
			case BY_nop: //00
				//Do nothing! that was easy
				break;
			case BY_aconst_null: //01
				pushInt(const_null);
				break;
			case BY_iconst_m1: //02
				pushInt(-1);
				break;
			case BY_iconst_0: //03
				pushInt(0);
				break;
			case BY_iconst_1: //04
				pushInt(1);
				break;
			case BY_iconst_2: //05
				pushInt(2);
				break;
			case BY_iconst_3: //06
				pushInt(3);
				break;
			case BY_iconst_4: //07
				pushInt(4);
				break;
			case BY_iconst_5: //08
				pushInt(5);
				break;
			case BY_lconst_0: //09
				pushLong(0);
				break;
			case BY_lconst_1: //0a
				pushLong(1);
				break;
			case BY_fconst_0: //0b
				pushFloat(0.0f);
				break;
			case BY_fconst_1: //0c
				pushFloat(1.0f);
				break;
			case BY_fconst_2: //0d
				pushFloat(2.0f);
				break;
			case BY_dconst_0: //0e
				pushDouble(0.0);
				break;
			case BY_dconst_1: //0f
				pushDouble(1.0);
				break;
			case BY_bipush: //10
				pushByte(GET_U1());
				break;
			case BY_sipush: //11
				pushShort(GET_U2());
				break;
			case BY_ldc: //12
				ldc(GET_U1());
				break;
			case BY_ldc_w: //13
				ldc(GET_U2());
				break;
			case BY_ldc2_w: //14
				{
					uint16_t index = GET_U2();
					if(constants.isType<ConstantLong>(index)) {
						ConstantLong& val = constants.get<ConstantLong&>(index);
						pushInt(val.getHighBits());
						pushInt(val.getLowBits());
					} else {
						ConstantDouble& val = constants.get<ConstantDouble&>(index);
						pushInt(val.getHighBits());
						pushInt(val.getLowBits());
					}
				}
				break;
			case BY_iload:
			case BY_fload:
			case BY_aload:
				load(GET_U1());
				break;
			case BY_lload:
			case BY_dload:
				load2(GET_U1());
				break;
			case BY_iload_0:
			case BY_fload_0:
			case BY_aload_0:
				load(0);
			case BY_lload_0:
			case BY_dload_0:
				load2(0);
				break;
			case BY_iload_1:
			case BY_fload_1:
			case BY_aload_1:
				load(1);
			case BY_lload_1:
			case BY_dload_1:
				load2(1);
				break;
			case BY_iload_2:
			case BY_fload_2:
			case BY_aload_2:
				load(2);
			case BY_lload_2:
			case BY_dload_2:
				load2(2);
				break;
			case BY_iload_3:
			case BY_fload_3:
			case BY_aload_3:
				load(3);
			case BY_lload_3:
			case BY_dload_3:
				load2(3);
				break;
			case BY_iaload:
			case BY_faload:
			case BY_aaload:
			case BY_baload:
			case BY_caload:
			case BY_saload:
			{
				uint32_t index = popInt();
				JavaArray& arr = vm.getJavaArray(popInt());
				pushInt(arr.getValue<int32_t>(index));
			}
				break;
			case BY_laload:
			case BY_daload:
			{
				uint32_t index = popInt();
				JavaArray& arr = vm.getJavaArray(popInt());
				pushLong(arr.getValue<int64_t>(index));
			}
				break;
			case BY_istore:
			case BY_fstore:
			case BY_astore:
				store(GET_U1());
				break;
			case BY_lstore:
			case BY_dstore:
				store2(GET_U1());
				break;
			case BY_istore_0:
			case BY_fstore_0:
			case BY_astore_0:
				store(0);
			case BY_lstore_0:
			case BY_dstore_0:
				store2(0);
				break;
			case BY_istore_1:
			case BY_fstore_1:
			case BY_astore_1:
				store(1);
			case BY_lstore_1:
			case BY_dstore_1:
				store2(1);
				break;
			case BY_istore_2:
			case BY_fstore_2:
			case BY_astore_2:
				store(2);
			case BY_lstore_2:
			case BY_dstore_2:
				store2(2);
				break;
			case BY_istore_3:
			case BY_fstore_3:
			case BY_astore_3:
				store(3);
			case BY_lstore_3:
			case BY_dstore_3:
				store2(3);
				break;
			case BY_iastore:
			case BY_fastore:
			case BY_aastore:
			case BY_bastore:
			case BY_castore:
			case BY_sastore:
			{
				uint32_t value = popInt();
				uint32_t index = popInt();
				JavaArray& arr = vm.getJavaArray(popInt());
				arr.setValue<int32_t>(index,value);
			}
				break;
			case BY_lastore:
			case BY_dastore:
			{
				int64_t value = popLong();
				uint32_t index = popInt();
				JavaArray& arr = vm.getJavaArray(popInt());
				arr.setValue<int64_t>(index,value);
			}
				break;
			case BY_pop:
				popInt();
				break;
			case BY_pop2:
				popLong();
				break;
			case BY_dup:
			{
				int32_t val = popInt();
				pushInt(val); pushInt(val);
			}
				break;
			case BY_dup_x1:
			{
				int32_t val1 = popInt();
				int32_t val2 = popInt();
				pushInt(val1); pushInt(val2); pushInt(val1);
			}
				break;
			case BY_dup_x2:
			{
				int32_t val1 = popInt();
				int32_t val2 = popInt();
				int32_t val3 = popInt();
				pushInt(val1); pushInt(val3); pushInt(val2); pushInt(val1);
			}
				break;
			case BY_dup2:
			{
				int64_t val = popLong();
				pushLong(val); pushLong(val);
			}
				break;
			case BY_dup2_x1:
			{
				int64_t val1 = popLong();
				int32_t val2 = popInt();
				pushLong(val1); pushInt(val2); pushLong(val1);
			}
				break;
			case BY_dup2_x2:
			{
				int64_t val1 = popLong();
				int64_t val2 = popLong();
				pushLong(val1); pushLong(val2); pushLong(val1);
			}
				break;
			case BY_swap:
			{
				int32_t val1 = popInt();
				int32_t val2 = popInt();
				pushInt(val1); pushInt(val2);
			}
				break;
			case BY_iadd:
				pushInt(popInt()+popInt());
				break;
			case BY_ladd:
				pushLong(popLong()+popLong());
				break;
			case BY_fadd:
				pushFloat(popFloat()+popFloat());
				break;
			case BY_dadd:
				pushDouble(popDouble()+popDouble());
				break;
			case BY_isub:
			{
				int32_t val2 = popInt();
				int32_t val1 = popInt();
				pushInt(val1-val2);
			} 
				break;
			case BY_lsub:
			{
				int64_t val2 = popLong();
				int64_t val1 = popLong();
				pushLong(val1-val2);
			}
			case BY_fsub:
			{
				float val2 = popFloat();
				float val1 = popFloat();
				pushFloat(val1-val2);
			}
				break;
			case BY_dsub:
			{
				double val2 = popDouble();
				double val1 = popDouble();
				pushDouble(val1-val2);
			}
				break;
			case BY_imul:
				pushInt(popInt()*popInt());
				break;
			case BY_lmul:
				pushLong(popLong()*popLong());
				break;
			case BY_fmul:
				pushFloat(popFloat()*popFloat());
				break;
			case BY_dmul:
				pushDouble(popDouble()*popDouble());
				break;
			case BY_idiv:
			{
				int32_t val2 = popInt();
				int32_t val1 = popInt();
				pushInt(val1/val2);
			}
				break;
			case BY_ldiv:
			{
				int64_t val2 = popLong();
				int64_t val1 = popLong();
				pushLong(val1/val2);
			}
				break;
			case BY_fdiv:
			{
				float val2 = popFloat();
				float val1 = popFloat();
				pushFloat(val1/val2);
			}
				break;
			case BY_ddiv:
			{
				double val2 = popDouble();
				double val1 = popDouble();
				pushDouble(val1/val2);
			}
				break;
			case BY_irem:
			{
				int32_t val2 = popInt();
				int32_t val1 = popInt();
				pushInt(val1%val2);
			}
				break;
			case BY_lrem:
			{
				int64_t val2 = popLong();
				int64_t val1 = popLong();
				pushLong(val1%val2);
			}
				break;
			case BY_frem:
			{
				float val2 = popFloat();
				float val1 = popFloat();
				pushFloat(fmod(val1,val2));
			}
				break;
			case BY_drem:
			{
				double val2 = popDouble();
				double val1 = popDouble();
				pushDouble(fmod(val1,val2));
			}
				break;
			case BY_ineg:
				pushInt(-popInt());
				break;
			case BY_lneg:
				pushLong(-popLong());
				break;
			case BY_fneg:
				pushFloat(-popFloat());
				break;
			case BY_dneg:
				pushDouble(-popDouble());
				break;
			case BY_ishl:
			{
				int32_t val2 = popInt();
				int32_t val1 = popInt();
				pushInt(val1<<val2);
			}
				break;
			case BY_lshl:
			{
				int32_t val2 = popInt();
				int64_t val1 = popLong();
				pushLong(val1<<val2);
			}
				break;
			case BY_ishr:
			{
				int32_t val2 = popInt();
				int32_t val1 = popInt();
				pushInt(val1>>val2);
			}
				break;
			case BY_lshr:
			{
				int32_t val2 = popInt();
				int64_t val1 = popLong();
				pushLong(val1>>val2);
			}
				break;
			case BY_iushr:
			{
				uint32_t val2 = popInt();
				uint32_t val1 = popInt();
				pushInt(val1>>val2);
			}
				break;
			case BY_iand:
				pushInt(popInt()&popInt());
				break;
			case BY_land:
				pushLong(popLong()&popLong());
				break;
			case BY_ior:
				pushInt(popInt()|popInt());
				break;
			case BY_lor:
				pushLong(popLong()|popLong());
				break;
			case BY_ixor:
				pushInt(popInt()^popInt());
				break;
			case BY_lxor:
				pushLong(popLong()^popLong());
				break;
			case BY_iinc:
			{
				u1 index = GET_U1();
				u1 add = GET_U1();
				localVariables[index]+=add;
			}
				break;
			case BY_i2l:
				pushLong(popInt());
				break;
			case BY_i2f:
				pushFloat(popInt());
				break;
			case BY_i2d:
				pushDouble(popInt());
				break;
			case BY_l2i:
				pushInt(popLong());
				break;
			case BY_l2f:
				pushFloat(popLong());
				break;
			case BY_l2d:
				pushDouble(popLong());
				break;
			case BY_f2i:
				pushFloat(popInt());
				break;
			case BY_f2l:
				pushLong(popFloat());
				break;
			case BY_f2d:
				pushDouble(popFloat());
				break;
			case BY_d2i:
				pushInt(popDouble());
				break;
			case BY_d2l:
				pushLong(popDouble());
				break;
			case BY_d2f:
				pushFloat(popDouble());
				break;
			case BY_i2b:
				pushInt((popInt()<<24)>>24);
				break;
			case BY_i2s:
				pushInt((popInt()<<16)>>16);
				break;
			case BY_lcmp:
			{
				int64_t val2 = popLong();
				int64_t val1 = popLong();
				if(val1>val2) { pushInt(1); }
				else if(val1==val2) { pushInt(0); }
				else { pushInt(-1); }
			}
				break;
			case BY_fcmpl: //TODO: fix this for NaN
			case BY_fcmpg:
			{
				float val2 = popFloat();
				float val1 = popFloat();
				if(val1>val2) { pushInt(1); }
				else if(val1==val2) { pushInt(0); }
				else { pushInt(-1); }
			}
				break;
			case BY_dcmpl: //TODO: fix this for NaN
			case BY_dcmpg:
			{
				double val2 = popDouble();
				double val1 = popDouble();
				if(val1>val2) { pushInt(1); }
				else if(val1==val2) { pushInt(0); }
				else { pushInt(-1); }
			}
				break;
			case BY_ifeq:
				if(popInt()==0) {
					location += (int16_t)(GET_U2()-3);
				} else {
					location += 2;
				}
				break;
			case BY_ifne:
				if(popInt()!=0) {
					location += (int16_t)(GET_U2()-3);
				} else {
					location += 2;
				}
				break;
			case BY_iflt:
				if(popInt()<0) {
					location += (int16_t)(GET_U2()-3);
				} else {
					location += 2;
				}
				break;
			case BY_ifge:
				if(popInt()>=0) {
					location += (int16_t)(GET_U2()-3);
				} else {
					location += 2;
				}
				break;
			case BY_ifgt:
				if(popInt()>0) {
					location += (int16_t)(GET_U2()-3);
				} else {
					location += 2;
				}
				break;
			case BY_ifle:
				if(popInt()<=0) {
					location += (int16_t)(GET_U2()-3);
				} else {
					location += 2;
				}
				break;
			case BY_if_acmpeq:
			case BY_if_icmpeq:
			{
				int32_t val2 = popInt();
				int32_t val1 = popInt();
				if(val1==val2) {
					location += (int16_t)(GET_U2()-3);
				} else {
					location += 2;
				}
			}
				break;
			case BY_if_acmpne:
			case BY_if_icmpne:
			{
				int32_t val2 = popInt();
				int32_t val1 = popInt();
				if(val1!=val2) {
					location += (int16_t)(GET_U2()-3);
				} else {
					location += 2;
				}
			}
				break;
			case BY_if_icmplt:
			{
				int32_t val2 = popInt();
				int32_t val1 = popInt();
				if(val1<val2) {
					location += (int16_t)(GET_U2()-3);
				} else {
					location += 2;
				}
			}
				break;
			case BY_if_icmpge:
			{
				int32_t val2 = popInt();
				int32_t val1 = popInt();
				if(val1>=val2) {
					location += (int16_t)(GET_U2()-3);
				} else {
					location += 2;
				}
			}
				break;
			case BY_if_icmpgt:
			{
				int32_t val2 = popInt();
				int32_t val1 = popInt();
				if(val1<val2) {
					location += (int16_t)(GET_U2()-3);
				} else {
					location += 2;
				}
			}
				break;
			case BY_if_icmple:
			{
				int32_t val2 = popInt();
				int32_t val1 = popInt();
				if(val1<=val2) {
					location += (int16_t)(GET_U2()-3);
				} else {
					location += 2;
				}
			}
				break;
			case BY_goto:
			{
				int16_t jump = GET_U2();
				location += jump-3;
			}
				break;
			case BY_jsr:
			{
				pushInt(location-1);
				int16_t jump = GET_U2();
				location += jump-3;
			}
				break;
			case BY_ret:
			{
				int32_t jump = localVariables[GET_U1()];
				location += jump-2;
			}
				break;
			case BY_tableswitch:
			{
				//freak out
				cout << "i don't wanna tableswitch this" << endl;
				exit(1);
			}
				break;
			case BY_lookupswitch:
			{
				//freak out
				cout << "i don't wanna lookupswitch this" << endl;
				exit(1);
			}
				break;
				
				
			default:
				cout << "unexpected instruction " << hex << opcode << dec << endl;
				exit(1);
			break;
		}
	}
}

void Frame::ldc(u2 index) {
	Constant& c = constants[index];
	switch(c.getType()) {
		case CONSTANT_Integer:
			pushInt(constants.get<ConstantInteger&>(index).getIntValue());
			break;
		case CONSTANT_Float:
			pushFloat(constants.get<ConstantFloat&>(index).getFloatValue());
			break;
		default:
			throw "i don't like yer constant";
			break;
	}
}