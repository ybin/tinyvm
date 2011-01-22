#ifndef VIRTUALMEMORY_H_
#define VIRTUALMEMORY_H_

#include <stdlib.h>

typedef struct
{
	// System memory
	int* int32;

} memory;

memory* create_memory(size_t size);
void destroy_memory(memory* mem);

#endif
