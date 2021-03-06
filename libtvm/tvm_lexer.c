#include <tvm/tvm_lexer.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

tvm_lexer_t *lexer_create()
{
	return (tvm_lexer_t *)calloc(1, sizeof(tvm_lexer_t));
}

void lexer_destroy(tvm_lexer_t *lexer)
{
	for(int i = 0; lexer->source_lines[i]; i++) free(lexer->source_lines[i]);
	free(lexer->source_lines);

	for(int i = 0; lexer->tokens[i]; i++)
	{
		for(int j = 0; j < MAX_TOKENS; j++)
			free(lexer->tokens[i][j]);

		free(lexer->tokens[i]);
	}

	free(lexer->tokens);
	free(lexer);
}

void lex(tvm_lexer_t *lexer, char *source, tvm_htab_t *defines)
{
	int i, j;
	char *pToken;
	char *pLine = strtok(source, "\n");

	/* Split the source into individual lines */
	for(i = 0; pLine; i++)
	{
		// 每拿到一行就要修改字符串数组的大小
		lexer->source_lines = (char **)realloc(lexer->source_lines, sizeof(char *) * (i + 2));
		// 因为是realloc，i之前的行都不动，所以只需分配i即可
		lexer->source_lines[i] = (char *)calloc(1, strlen(pLine) + 1);

		strcpy(lexer->source_lines[i], pLine);

		/* Ignore comments delimited by '#' */
		char* comment_delimiter = strchr(lexer->source_lines[i], '#');
		// 去掉单行注释的方法就是把注释对应的指针置为NULL
		if(comment_delimiter) *comment_delimiter = 0; // a.k. comment_delimiter = '\0'

		// 再拿一行
		pLine = strtok(NULL, "\n");
	}

	/* NULL terminate the array to make iteration later easier*/
	lexer->source_lines[i] = NULL;

	/* Split the source into individual tokens */
	for(i = 0; lexer->source_lines[i]; i++)
	{
		lexer->tokens = (char ***)realloc(lexer->tokens, sizeof(char **) * (i + 2));
		lexer->tokens[i] = (char **)calloc(MAX_TOKENS, sizeof(char *));

		// token直接用空白(space or tab)分割
		pToken = strtok(lexer->source_lines[i], " \t,");

		for(j = 0; (pToken && j < MAX_TOKENS); j++)
		{
			char *token = htab_find_ref(defines, pToken);
			token = token ? token : pToken;

			lexer->tokens[i][j] = (char *)calloc(1, (strlen(token) + 1));
			strcpy(lexer->tokens[i][j], token);

			pToken = strtok(NULL, " \t,");
		}
	}

	lexer->tokens[i] = NULL;
	htab_destroy(defines);
}
