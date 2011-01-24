#ifndef VIRTUALMEMORY_H_
#define VIRTUALMEMORY_H_

#include <stdlib.h>

typedef union
{
	// 64-bit support is not a priority right now
	//long long i64;
	int i32;
	short i16;
	char i8[2];
} reg;

typedef struct
{
	// System memory
	int* int32;

	/*   -- REGISTERS --
		0 - E/A/X/H/L		Accumulator register
		1 - E/B/X/H/L		Base register
		2 - E/C/X/H/L		Counter Register
		3 - E/D/X/H/L		Data register
		4 - E/SI		Source index
		5 - E/DI		Destination Index
     *unused*	6 - E/BP		Base pointer
     *unused*	7 - E/SP		Stack pointer		*/

	reg registers[8];

} memory;

memory* create_memory(size_t size);
void destroy_memory(memory* mem);

#endif
