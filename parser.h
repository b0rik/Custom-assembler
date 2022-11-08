#ifndef PARSER_H
#define PARSER_H

#include "defs.h"
#include "error.h"
#include "symtable.h"

/*enum of types of lines*/
typedef enum{
	MACRO_TYPE,
	DIRECTIVE_TYPE,
	INSTRUCTION_TYPE,
	UNDEF_TYPE
}line_type;

/*a struct representing a parsed line*/
typedef struct{
	char line[MAX_LINE_LEN + 1];
	char label[MAX_LABEL_LEN + 1];
	char name[MAX_LABEL_LEN + 1];
	int macro_value;
	line_type type;
	char parameters[MAX_LINE_LEN + 1];
}parsed_line;

int get_num_of_params(parsed_line*);
error_value parse_line(parsed_line*, symtable*);
void reset_parsed_line(parsed_line*);
error_value parse_macro_name(parsed_line*, char*, symtable*);
error_value parse_macro_param(parsed_line*, char*);

#endif
