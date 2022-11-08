#include "error.h"

void print_error(error_value err_val, const char *file_name, const int index) {
	switch (err_val) {
	case NO_PARAMETERS:
		printf("no parameters provided\n");
		break;
	case INVALID_FILE_NAME:
		printf("%s: file doesn't exist\n", file_name);
		break;
	case ERROR_MEMORY_ALLOC:
		printf("%s: unable to allocate memory\n", file_name);
		break;
	case LINE_TOO_LONG:
		printf("%s: %d: line too long\n", file_name, index);
		break;
	case LABEL_TOO_LONG:
		printf("%s: %d: the label is too long\n", file_name, index);
		break;
	case INVALID_LABEL:
		printf("%s: %d: the label is illegal\n", file_name, index);
		break;
	case DUPLICATE_LABEL:
		printf("%s: %d: label already exists\n", file_name, index);
		break;
	case INVALID_DIRECTIVE:
		printf("%s: %d: invalid directive\n", file_name, index);
		break;
	case INVALID_INSTRUCTION:
		printf("%s: %d: invalid instruction\n", file_name, index);
		break;
	case RESERVED_WORD:
		printf("%s: %d: symbol is a reserved word\n", file_name, index);
		break;
	case SYNTAX_ERROR:
		printf("%s: %d: syntax error\n", file_name, index);
		break;
	case INVALID_MACRO:
		printf("%s: %d: the macro is illegal\n", file_name, index);
		break;
	case NOT_A_NUMBER:
		printf("%s: %d: parameter is not a legal number\n", file_name, index);
		break;
	case MACRO_TOO_LONG:
		printf("%s: %d: the macro name is too long\n", file_name, index);
		break;
	case MACRO_AFTER_LABEL:
		printf("%s: %d: macro and lable in the same line is not allowed\n",
			   file_name, index);
		break;
	case INVALID_PARAMETERS:
		printf("%s: %d: invalid parameters to operation\n", file_name, index);
		break;
	case DUPLICATE_MACRO:
		printf("%s: %d: macro name already defined\n", file_name, index);
		break;
	case INVALID_STRING:
		printf("%s: %d: the string is invalid\n", file_name, index);
		break;
	case INVALID_NUM_OF_PARAMS:
		printf("%s: %d: invalid number of parameters for operation\n",
			   file_name, index);
		break;
	case MACRO_PARAM_UNDEFINED:
		printf("%s: %d: the macro data parameter in not defined yet\n",
	           file_name, index);
		break;
	case INVALID_PARAMETER:
		printf("%s: %d: invalid parameter to operation\n", file_name, index);
		break;
	case INVALID_ADDR_SRC_MODE:
		printf("%s: %d: the operation doesn't support this addressing source mode\n",
			   file_name, index);
		break;
	case INVALID_ADDR_DEST_MODE:
		printf("%s: %d: the operation doesn't support this addressing destination mode\n",
			   file_name, index);
		break;
	case ERROR_OPEN_FILE:
		printf("%s: unable to open file\n", file_name);
		break;
	case ERROR_PASS1:
		printf("%s: first pass failed\n", file_name);
		break;
	case ERROR_PASS2:
		printf("%s: second pass failed\n", file_name);
		break;
	case ENTRY_UNDEFINED:
		printf("%s: %d: the entry point is undefined\n", file_name, index);
		break;
	case LABEL_UNDEF:
		printf("%s: %d: the label parameter is undefined\n", file_name, index);
		break;
	case ERROR_CREATE_FILE:
		printf("%s: failed to create a file\n", file_name);
		break;
	case NO_ERROR:
		printf("%s: processed no error\n", file_name);
		break;
	case NO_MACRO_PARAM:
		printf("%s: %d: macro parameter is no provided\n", file_name, index);
		break;
	case EMPTY_LABEL:
		printf("%s: %d: empty label declared\n", file_name, index);
		break;
	default:
		printf("%s: encountered an unexpected error", file_name);
	}
}
