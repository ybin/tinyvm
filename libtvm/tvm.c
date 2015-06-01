#include <tvm/tvm.h>

// 创建VM
tvm_t *tvm_create(char *filename)
{
	tvm_t *vm = (tvm_t*)calloc(1, sizeof(tvm_t));

	vm->pMemory = memory_create(MIN_MEMORY_SIZE);
	vm->pProgram = program_create();
	stack_create(vm->pMemory, MIN_STACK_SIZE);

	if(!vm || !vm->pMemory || !vm->pProgram) return NULL;

	return vm;
}

// 加载程序进内存
int tvm_interpret(tvm_t *vm, char *filename)
{
	if(program_interpret(vm->pProgram, filename, vm->pMemory) != 0) return 1;
	return 0;
}

void tvm_destroy(tvm_t *vm)
{
	if(vm && vm->pMemory) memory_destroy(vm->pMemory);
	if(vm && vm->pProgram) program_destroy(vm->pProgram);
	if(vm) free(vm);
}

// 程序的解释执行
void tvm_run(tvm_t *vm)
{
	int *instr_idx = &vm->pMemory->registers[0x8].i32; // 0x8是哪个寄存器?
	*instr_idx = vm->pProgram->start; // instruct从start开始，即从main()函数开始

	for(;vm->pProgram->instr[*instr_idx] != -0x1; ++(*instr_idx)) tvm_step(vm, instr_idx);
}

