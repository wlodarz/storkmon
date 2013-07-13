
#include "zbparse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct zb_response_1words {
	zb_response_code code;
	char *word;
} zb_resp_1words[] = {
	{ ZB_RESP_CODE_TEST1, "TEST1" },
	{ ZB_RESP_CODE_TEST2, "TEST2" },
	{ ZB_RESP_CODE_LAST, "LAST" }
};

static int string_tokenize(char *src, char *tokens[], int max_tokens);

int zbparser_init(void)
{
	return 0;
}

/* parses ZigBee modem response line
 * fills tokens[] with pointers to start of every token/word
 * returns number of found tokens, or -1 if not found
 */
int zbparser_parse(char *in_text, char *tokens[], int max_tokens)
{
	int i, tokens_found;

	tokens_found = string_tokenize(in_text, tokens, max_tokens);
	for(i=0; i<tokens_found; i++) {
		printf("token[%d] = %s\n", i, tokens[i]);
	}

	i=0;
	while(   zb_resp_1words[i].code != ZB_RESP_CODE_LAST
	      && strcmp(zb_resp_1words[i].word, tokens[0]) != 0) {
		i++;
	}

	return zb_resp_1words[i].code;
}

int zbparser_destroy(void)
{
	return 0;
}

/* split string 'src' into tokens[] 
 */
static int string_tokenize(char *src, char *tokens[], int max_tokens)
{
	int token_num = 0;
	int parsing_token = 0; /* while found new token, parsing_token is set to 1. while parsing whitespaces it is set to 0 */

	while(src && *src != '\0' && token_num < max_tokens) {
		if (   *src != ' '
		   ) {
			/* parse not whitespace */
			if (!parsing_token) {
				/* found new token */
				tokens[token_num++] = src;
				parsing_token = 1;
			}
		} else {
			/* parse whitespace */
			parsing_token = 0;
			*src = '\0';
		}
		src++;
	}

	return token_num;
}

