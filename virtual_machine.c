#include "virtual_machine.h"

virtual_machine* create_vm(char* filename)
{
	virtual_machine* vm;

	vm = (virtual_machine*)malloc(sizeof(virtual_machine));

	vm->pStack = create_stack();
	vm->pMemory = create_memory(512000);
	vm->pProgram = create_program(filename, vm->pMemory);

	// Make sure the program was interpreted properly
	if(!vm->pProgram) return NULL;

	return vm;
}

void destroy_vm(virtual_machine* vm)
{
	if(vm)
	{
		if(vm->pMemory) destroy_memory(vm->pMemory);
		if(vm->pStack) destroy_stack(vm->pStack);
		if(vm->pProgram) destroy_program(vm->pProgram);

		free(vm);
	}
}

void run_vm(virtual_machine* vm)
{
	register int instr_idx = vm->pProgram->start;
	register int equal = 0, greater = 0, remainder = 0;

	register int **arg0, **arg1;
	arg0 = &vm->pProgram->args[0][instr_idx];
	arg1 = &vm->pProgram->args[1][instr_idx];

	while(vm->pProgram->instr[instr_idx] != END)
	{
		switch(vm->pProgram->instr[instr_idx])
		{
		case MOV:
			**arg0 = **arg1;
			break;
		case PUSH:
			stack_push(vm->pStack, *arg0);
			break;
		case POP:
			stack_pop(vm->pStack, *arg0);
			break;
		case INC:
			++(**arg0);
			break;
		case DEC:
			--(**arg0);
			break;
		case ADD:
			**arg0 += **arg1;
			break;
		case SUB:
			**arg0 -= **arg1;
			break;
		case MUL:
			**arg0 *= **arg1;
			break;
		case DIV:
			**arg0 /= **arg1;
			break;
		case MOD:
			remainder = **arg0 % **arg1;
			break;
		case REM:
			**arg0 = remainder;
			break;
		case NOT:
			**arg0 = ~(**arg0);
			break;
		case XOR:
			**arg0 ^= **arg1;
			break;
		case OR:
			**arg0 |= **arg1;
			break;
		case AND:
			**arg0 &= **arg1;
			break;
		case SHL:
			**arg0 <<= **arg1;
			break;
		case SHR:
			**arg0 >>= **arg1;
			break;
		case CMP:
			equal = (**arg0 == **arg1);
			greater = (**arg0 > **arg1);
			break;
jmp:		case JMP:
			instr_idx = **arg0 - 1;
			arg0 = &vm->pProgram->args[0][instr_idx];
			arg1 = &vm->pProgram->args[1][instr_idx];
			break;
		case JE:
			if(equal) goto jmp;;
			break;
		case JNE:
			if(!equal) goto jmp;
			break;
		case JG:
			if(greater) goto jmp;
			break;
		case JGE:
			if(greater || equal) goto jmp;
			break;
		case JL:
			if(!greater && !equal) goto jmp;
			break;
		case JLE:
			if(!greater) goto jmp;
			break;
		}

		++instr_idx;
		++arg0;
		++arg1;
	}
}
