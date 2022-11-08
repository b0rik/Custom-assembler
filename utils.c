#include "utils.h"
#include "memory_image.h"

/* line_valid : check if a line is a valid line
 * parameters : line - the line to check
 * return     : LINE_TOO_LONG     - if the line is too long
 * 				NO_ERROR          - if the line is valid
 * 			    INVALI_PARAMETERS - if line is NULL*/
error_value line_valid(const char *line){
	if(line)
		/*check if the line is too long*/
		return strlen(line) == MAX_LINE_LEN + 1 && line[MAX_LINE_LEN] != '\n' ? LINE_TOO_LONG : NO_ERROR;
	else
		return INVALID_PARAMETERS;
}

/* is_empty_line : check if the line is empty
 * parameters    : line - the line to check
 * return        : TRUE  - if the line is empty
 * 				   FALSE - if the line is not empty*/
int is_empty_line(const char *line){
	int len = strlen(line),
		i,
		empty = line ? TRUE : FALSE;

	/*itterate over the line and look for a non space character*/
	for(i = 0; i < len && empty; i++)
		if(!isspace(line[i]))
			empty = FALSE;

	return empty;
}

/* is_comment_line : check if the line is a comment line
 * parameters      : line - the line to check
 * return          : TRUE  - if the line is a comment line
 * 					 FALSE - if the line is not a comment line*/
int is_comment_line(const char *line){
	const char *p = line;
	int 	   len = strlen(line), i;

	/*look for the first non space line*/
	for(i = 0; i < len && isspace(*p); i++, p++);

	/*if its a ';' then its a comment line*/
	return *p == ';' ? TRUE : FALSE;
}

/* valid_label : chekc if a label is a valid label
 * parameters  : label      - the label to check
 * 				 symtable_p - a pointer to a symbol table
 * return      : NO_ERROR       - if the label is valid
 * 				 INVALID_LABEL  - if the label is invalid
 * 				 LABEL_TOO_LONG - if the label is too long
				 RESERVED_WORD  - if the label is a reserved word */
error_value valid_label(const char *label, symtable *symtable_p){
	int         i,
		        len = strlen(label);

	/*check if the first characther is a letter*/
	error_value err_val = len == 0 || isalpha(label[0]) ? NO_ERROR : INVALID_LABEL;

	/*check if the label is not too long*/
	if(!err_val && len > MAX_LABEL_LEN)
		err_val = LABEL_TOO_LONG;

	/*chekc if the label is alphanumeric*/
	for(i = 1; !err_val && i < len; i++){
		if(!isalnum(label[i]))
			err_val = INVALID_LABEL;
	}

	/*check if the label is a reserved word */
	err_val = !err_val && is_reserved_word(label) ? RESERVED_WORD : err_val;

	return err_val;
}

/* valid_instruction : check if an instruction is a valid instruction
 * parameters        : str - the string to check
 * return            : NO_ERROR           - if the instruction is valid
 * 					   INVALID_INSTRCTION - if the string is invalid instruciton*/
error_value valid_instruction(const char *str){
	return get_op_value(str) >= 0 ? NO_ERROR : INVALID_INSTRUCTION;
}

/* is reserved_word : check if a string is a reserved word
 * parameters       : str - the string to check
 * return           : non zero value if the string is a reserved word
 * 				      else return zero*/
int is_reserved_word(const char *str){
	return get_op_value(str) < 0 &&
		   valid_directive(str) &&
		   strcmp(str, ".define") &&
		   get_reg_val(str) < 0 ? FALSE : TRUE;
}

/* valid_macro : check if a string is a valid macro name
 * parameters  : macro      - the string to check
 * 				 symtable_p - a pointer to a symbol table
 * return      : NO_ERROR        - if the string is a valid macri name
 * 				 INVALID MACRO   - if the macro namee is invalid
 * 				 MACRO_TOO_LONG  - if the macro name is too long
 * 				 DUPLICATE_MACRO - if the macro name has already been defined*/
error_value valid_macro_name(const char *macro, symtable *symtable_p){
	/*check if the first character is a letter*/
	error_value err_val = isalpha(macro[0]) ? NO_ERROR : INVALID_MACRO;
	int 		i;

	/*check if the name is too long*/
	if(!err_val && strlen(macro) > MAX_LABEL_LEN)
		err_val = MACRO_TOO_LONG;

	/*check if the name is alphanumeric	*/
	for(i = 0; !err_val && i < strlen(macro); i++)
		if(!isalnum(macro[i]))
			err_val = INVALID_MACRO;

	/*check if the macro already defined and return the result*/
	return !err_val && find_symbol(macro, symtable_p) ? DUPLICATE_MACRO : err_val;
}

/* is_legal_number : check if a string is a legal number
 * parameters      : str - the string to check
 * return          : non zero value if the string is a valid number
 * 					 else return zero*/
int is_legal_number(const char *str){
	/*check if the first character is a '-' , '+' or a digit*/
	int legal = str[0] == '-' || str[0] == '+' || isdigit(str[0]) ? TRUE : FALSE,
	    i;

	/*check if all the character*/
	for(i = 1; legal && i < strlen(str); i++)
		if(!isdigit(str[i]))
			legal = FALSE;

	return legal;
}

/* is_valid_string : check if a string is valid and printable
 * parameters      : str - the string to check
 * return          : non zero value if the string is valid and printable
 * 					 else return zero*/
int is_valid_string(const char *str){
	int i,
	    len = strlen(str),
		/*check is the string has ' " ' and the begginning and end*/
	    valid = len >= 2 && str[0] == '"' && str[len - 1] == '"' ? TRUE : FALSE;

	/*check if every character is a alphanumeric or space*/
	for(i = 1; valid && i < len - 1; i++)
		if(!isalnum(str[i]) && !isspace(str[i]))
			valid = FALSE;

	return valid;
}

/* is_valid_data_params : check if the string is a valid data parameters string
 * parameters           : str        - the string to check
 * 						  symtable_p - a pointer to a symbol table
 * return               : non zero value if the string a valid data parameters
 * 						  else return zeor*/
int is_valid_data_params(const char *str, symtable *symtable_p){
	int  len = strlen(str);
	int  valid = TRUE;
	char *temp,
		 param_cpy[MAX_LINE_LEN];

	/*create a copy of the string to no alter the original */
	strcpy(param_cpy, str);

	/*check if theres a comma at the end or begginigs*/
	if(len >= 2){
		valid = str[0] == ',' || str[len - 1] == ',' ? FALSE : TRUE;

		/*check if there adjecent commasd in the string */
		if(valid)
			valid = !is_adjacent_commas(str);


		/*check if every parameter is a number of a defined macro*/
		for(temp = strtok(param_cpy, ","); valid && temp; temp = strtok(NULL, ","))
			if(!is_legal_number(temp) && !find_symbol(temp, symtable_p))
				valid = FALSE;

	} else if(len == 1 && !isdigit(str[0]))
		valid = FALSE;

	return valid;
}

/* is_adjacent_commas : chekc if there are adjacent commas on a string
 * parameters         : str - the string to check
 * return             : non zero value if there are adjacent commas
 *                      else return zero*/
int is_adjacent_commas(const char *str){
	int i,
		len = strlen(str),
		adj_comma = FALSE;

	/*check if there are two adjacent commas*/
	for(i = 0; adj_comma && i < len - 1; i++){
		if(str[i] == ',' && str [i + 1] == ',')
			adj_comma = TRUE;
	}

	return adj_comma;
}

/* is_valid_instruction_param : check if the parameter is a valid parameter for an instruction
 * parameters                 : param      - the parameter to check
 * 							    symtable_p - a pointer to a symbol table
 * return                     : non zero value if its a valid parameter
 * 								else return zero */
int is_valid_instruction_param(const char *param, symtable *symtable_p){
	int valid;

	/*check if it and empty parameter*/
	if(is_empty_line(param))
		valid = TRUE;
	/*check if its a number parameter or macro*/
	else if(param[0] == '#'){
		if(!(valid = is_legal_number(param + 1)))
			valid = find_macro(param + 1, symtable_p) ? TRUE : FALSE;
		/*check if the parameter is a register*/
	}else if(param[0] == 'r' && get_reg_val(param) >= 0)
		valid = TRUE;
	/*check if its an array parameter*/
	else if(is_array_param(param, symtable_p))
		valid = TRUE;
	else
		/*check if the parameter is a valid label*/
		valid = !valid_label(param, symtable_p);


	return valid;
}

/* is_array_param : check if a parameter is an array parameter
 * parameters     : param - the parameter to check
 * return         : non zero value if the parameter is a valid array parameter
 * 					else return zero*/
int is_array_param(const char *param, symtable *symtable_p){
	char *index,
		 *end, name[MAX_LABEL_LEN];
	int  is_array = FALSE;

	/*make a copy of the parameters to not alter the original*/
	strcpy(name, param);

	/*check if there are '[' and ']'*/
	if((index = strchr(name, '[')) && (end = strchr(name, ']'))){
		/*check of the '[' comes before the ']'*/
		if(index < end){

			*(index++) = '\0';
			*end = '\0';
			/*check if the name of the array ia a valid label*/
			if((is_array = !valid_label(name, symtable_p)))
				/*check if the index is a number or a valid macro*/
				is_array = is_legal_number(index) || find_macro(index, symtable_p);
		}
	}

	return is_array;
}

/* get_arr_name : get the name of an array parameter
 * parameters   : str_in  - the string of the array
 * 				  str_out - the output string for the name
 * return       :
 */
void get_arr_name(const char *str_in, char *str_out){
	strcpy(str_out, str_in);
	strchr(str_out, '[')[0] = '\0';
}

/* get_arr_index : get the index of an array parameter
 * parameters    : str_in  - the string with the array
 * 				   str_out - the output string for the index
 * return        :
 */
void get_arr_index(const char *str_in, char *str_out){
	char *start;

	start = strchr(str_in, '[');
	strcpy(str_out, ++start);
	strchr(str_out, ']')[0] = '\0';
}

/* find_eq_sign : ind an '=' sign in a string
 * parameters   : str - the string to look at
 * return       : if found then return a pointer to the '=' sign
 * 				  else return NULL*/
char *find_eq_sign(char *str){
	char *p = str;

	for(; *p; p++)
		if(*p == '=')
			break;

	return (*p == '=') ? p : NULL;
}
