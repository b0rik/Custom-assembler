#include "pass2.h"
#include "utils.h"
#include "encoder.h"

/* handle_entry : a function to handle and entry line in second pass
 * parameters   : token      - the parameter of the entry line
 * 				  symtable_p - a pointer to a symbol table
 * return       : NO_ERROR        - if no erro occured
				  ENTRY_UNDEFINED - if the entry label is undefined */
static error_value handle_entry(const char *token, symtable *symtable_p) {
	error_value    err_val = NO_ERROR;
    symtable_entry *entry;

    /*if the entry label is defined then flag it as entry label*/
	if ((entry = find_symbol(token, symtable_p)))
		entry->type = ENTRY;
	else
		err_val = ENTRY_UNDEFINED;

	return err_val;
}
/* pass2_handle_line : a function to handle a line in second pass and encode it
 * parameters        : line       - the line to handle
 * 					   symtable_p - a pointer to a symbol table
 * 					   mem_img    - a pointer to a memory image
 * return            : NO_ERROR        - if no error occured
 * 					   ENTRY_UNDEFINED - if an entry label parameter is undefined
 * 					   LABEL_UNDEF     - if an undefined label is used*/
static error_value pass2_handle_line(char *line, symtable *symtable_p,
							  memory_image *mem_img) {
	error_value err_val = NO_ERROR;
	char 	    *token;

	/*the the line is not empty and not a comment line*/
	if (!is_empty_line(line) && !is_comment_line(line)) {
		/*get the first token*/
		token = strtok(line, PARSING_WHITESPACE_TOKENS);

		/*if its a label skip it*/
		if (token[strlen(token) - 1] == ':')
			token = strtok(NULL, PARSING_WHITESPACE_TOKENS);

		/*if its a directive */
		if (token[0] == '.') {
			/*if its an entry line*/
			if (!strcmp(token, ".entry")) {
				/*get its parameter and handle it*/
				token = strtok(NULL, PARSING_WHITESPACE_TOKENS);
				if (!(err_val = handle_entry(token, symtable_p)))
					symtable_p->entry_flag = TRUE;
			}
		} else
			/*if its not a directive then its an instruction and we need to encode it*/
			err_val = encode_instruction(token, symtable_p, mem_img);
	}

	return err_val;
}

/* pass2_execute : a function that executes pass to on a given file
 * parameters    : fp         - a pointer to a file to proccess
 * 				   mem_img_p  - a pointer to a memory image
 * 				   symtable_p - a pointer to a symbol table
 * 				   file_name  - the name of the proccessed file
 * return        : NO_ERROR    - if no erro occured
 *  		       ERROR_PASS2 - if an error occured in the second pass*/
error_value pass2_execute(FILE *fp, memory_image *mem_img_p,
						  symtable *symtable_p, const char *file_name) {
	error_value err_val = NO_ERROR;
	int         i,
				err_flag = FALSE;
	char	    line[MAX_LINE_LEN + 2];

	/*itterate every line in the file and encode it*/
	for (i = 1; fgets(line, MAX_LINE_LEN + 2, fp); i++)
		if ((err_val = pass2_handle_line(line, symtable_p, mem_img_p))) {
			err_flag = TRUE;
			print_error(err_val, file_name, i);
		}

	return err_flag ? ERROR_PASS2 : NO_ERROR;
}

/* pass2_prep : a function to prepare for the second pass after we finish the first pass
 * parameters : fp         - a pointer to the proccessed file
 * 				mem_img    - a pointer to a memory image
 * 				symtable_p - a pointer to a symbol table
 * return     :
 */
void pass2_prep(FILE *fp, memory_image *mem_img, symtable *symtable_p) {
	/*after we finish the first pass we can update the addresses of all the data
	 * to be after the code */
	update_data_addr(mem_img->data, mem_img->code->ic);
	update_data_sym_values(symtable_p, mem_img->code->ic);
	mem_img->code->ic = 0;
	/*and we return to the begginning of the file to go over it again*/
	fseek(fp, 0, SEEK_SET);
}
