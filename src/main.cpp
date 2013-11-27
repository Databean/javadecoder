#include "ClassFile.h"
#include <iostream>
#include <stdio.h>

using namespace std;

int main(int argc, const char** argv) {
	VirtualMachine vm;
	//vm->getClass("java/lang/StringBuilder");
	
	try {
		//VirtualMachine* vm = new VirtualMachine();
		if(argc>1) {
			vm.setMainClass(argv[1]);
		} else {
			vm.setMainClass("java/lang/Object");
		}
		vm.runMain();
	} catch(const char* str) {
		cout << str << endl;
	} catch(string s) {
		cout << s << endl;
	}
}