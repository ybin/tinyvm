#ifndef TVM_LEXER_H_
#define TVM_LEXER_H_

#define MAX_ARGS 2
#define MAX_TOKENS 4

#include "tvm_htab.h"

typedef struct tvm_lexer_s
{
	char **source_lines; // ��Դ�ļ����зָ�
	char ***tokens; // ÿһ�ж������ж��token
} tvm_lexer_t;

tvm_lexer_t *lexer_create();
void lexer_destroy(tvm_lexer_t *l);

/* Tokenize the character array "source" into lines and tokens */
void lex(tvm_lexer_t *lexer, char *source, tvm_htab_t *defines);

#endif
