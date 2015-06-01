#include <tvm/tvm_file.h>
#include <tvm/tvm_program.h>
#include <tvm/tvm_preprocessor.h>
#include <tvm/tvm_lexer.h>
#include <tvm/tvm_parser.h>

tvm_program_t *program_create()
{
	tvm_program_t *p = (tvm_program_t *)calloc(1, sizeof(tvm_program_t));
	p->label_htab = htab_create();
	p->defines = htab_create();

	return p;
}

void program_destroy(tvm_program_t *p)
{
	htab_destroy(p->label_htab);

	if(p->values)
	{
		for(int i = 0; i < p->num_values; i++) free(p->values[i]);
		free(p->values);
	}

	if(p->args)
	{
		for(int i = 0; p->args[i]; i++) free(p->args[i]);
		free(p->args);
	}

	if(p->instr) free(p->instr);
	free(p);
}

int program_interpret(tvm_program_t *p, char *filename, tvm_memory_t *pMemory)
{
	FILE *pFile = NULL;
	int source_length = 0;

	/* Attempt to open the file. If the file cannot be opened, try once more. */
	if(filename)
		for(int i = 0; i < 2; i++)
			if(!pFile) pFile = tvm_fopen(filename, ".vm", "r");

	if(!pFile)
	{
	        printf("File was not found, or does not exist. Unable to interpret.\n");
	        return 1;
	}

	source_length = tvm_flength(pFile);
	char *source = calloc(source_length, sizeof(char));

	// �ļ��ڴ渴�ƽ��ڴ�
	tvm_fcopy(source, source_length, pFile);
	fclose(pFile);

	// Ԥ����
	int err = 0;
	while((err = tvm_preprocess(source, &source_length, p->defines)) > 0);

	/* The preprocessor encountered a problem. */
	if (err < 0)
		return 1;

	// �ʷ�����
	tvm_lexer_t *lexer_ctx = lexer_create();
	lex(lexer_ctx, source, p->defines); // lex()��Ŀ�ľ��ǰ�lexer_ctx(�ʷ���)����
	free(source);

	// �﷨����
	if(parse_labels(p, (const char ***)lexer_ctx->tokens) != 0) return 1; // ���p->label_htab
	if(parse_instructions(p, (const char ***)lexer_ctx->tokens, pMemory) != 0) return 1; // ���instruct, args, values��

	lexer_destroy(lexer_ctx);
	// ���֮�󣬳������ڴ��е����ݽṹ�ʹ���������
	return 0;
}

