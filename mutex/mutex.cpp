#include "mutex.h"

void enter_mutex(mutex* m) {
	int* flag = &m->value;
	//__asm {
	//wait_lock:
	//	tsl reg, flag
	//	cmp reg, 0
	//	jnz wait_lock;
	//	ret
	//}

	__asm {
	enter_region:
		MOV eax, 1
		MOV ebx, flag
		XCHG eax, dword ptr [ebx] 
		CMP eax, 0
		JNE enter_region
	}
}

void leave_mutex(mutex* m) {
	int* flag = &m->value;

	__asm {
		MOV eax, 0
		MOV ebx, flag
		XCHG dword ptr [ebx], eax
	}

	return;
}