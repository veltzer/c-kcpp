#include "services.h"

/* to satisfy the kernel dynamic linker */
/* These are only needed if you dont supply flags
to the compiler to remove exception handling, run time
type information and the like... */
int __gxx_personality_v0;
int _Unwind_Resume;
//int _GLOBAL_OFFSET_TABLE_;

/* support code for new and delete */
void *operator new(unsigned long x) {
	return(service_malloc(x));
}


void operator delete(void *pointer) {
	service_free(pointer);
}

void operator delete(void *pointer, const long unsigned int type) {
	service_free(pointer);
}
