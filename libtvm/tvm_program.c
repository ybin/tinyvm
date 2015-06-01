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

	// 文件内存复制进内存
	tvm_fcopy(source, source_length, pFile);
	fclose(pFile);

	// 预处理
	int err = 0;
	while((err = tvm_preprocess(source, &source_length, p->defines)) > 0);

	/* The preprocessor encountered a problem. */
	if (err < 0)
		return 1;

	// 词法分析
	tvm_lexer_t *lexer_ctx = lexer_create();
	lex(lexer_ctx, source, p->defines); // lex()的目的就是把lexer_ctx(词法树)填满
	free(source);

	// 语法分析
	if(parse_labels(p, (const char ***)lexer_ctx->tokens) != 0) return 1; // 填充p->label_htab
	if(parse_instructions(p, (const char ***)lexer_ctx->tokens, pMemory) != 0) return 1; // 填充instruct, args, values等

	lexer_destroy(lexer_ctx);
	// 完成之后，程序在内存中的数据结构就创建起来了
	return 0;
}

