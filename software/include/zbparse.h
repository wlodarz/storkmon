#ifndef __STORK_ZBPARSE_H_
#define __STORK_ZBPARSE_H_

enum zb_response_codes {
        ZB_RESP_CODE_TEST1 = 1,
        ZB_RESP_CODE_TEST2,
        ZB_RESP_CODE_LAST
};
typedef enum zb_response_codes zb_response_code;

/* The parser for ZigBee module output messages.
 */
int zbparser_init(void);

/* Parsing function
 */
int zbparser_parse(char *in_text, char *tokens[], int max_tokens);

/* Destroy ZigBee parser internal structures
 */
int zbparser_destroy(void);


#endif // __STORK_ZBPARSE_H_

