#include "parser.h"
#include "utils.h"
#include "memory_image.h"

/* parse_directive_params : a function to parse the parameters of a directive line
 * parameters             : line        - a pointer to a parsed line struct
 * 						    symtable_p  - a pointer to a symbol table
 * return                 : NO_ERROR           - if all the parameters are valid
 * 							INVALID_PARAMETERS - if the data or extern directive parameters are invalid
 * 							INVALID_STRING     - if the string directive parameter is invalid
 * 							DUPLICATE_LABEL    - if the extern directive parameter is invalid
 * 							INVALID_LABEL      - if the entry directive parameter is invalid
 * 							LABEL_TOO_LONG     - if the entry directive parameter is invalid
 * 							RESERVED_WORD      - if the entry directive parameter is invalid
 * 							*/
static error_value parse_directive_params(parsed_line *line, symtable *symtable_p) {
	error_value err_val = NO_ERROR;

	/*if data directive check if valid parameters*/
	if (!strcmp(line->name, ".data")) {
		if (!is_valid_data_params(line->parameters, symtable_p))
			err_val = INVALID_PARAMETERS;

		/*if string directive check if valid parameters*/
	} else if (!strcmp(line->name, ".string")) {
		if (!is_valid_string(line->parameters))
			err_val = INVALID_STRING;

		/*if extern directive check if valid parameters*/
	} else if (!strcmp(line->name, ".extern")) {
		if (valid_label(line->parameters, symtable_p))
			err_val = INVALID_PARAMETERS;
		else if (find_symbol(line->parameters, symtable_p))
			err_val = DUPLICATE_LABEL;

		/*if entry directive check if valid parameters*/
	} else
		err_val = valid_label(line->parameters, symtable_p);

	return err_val;
}

/* parase_instruction_params : a function to parse the parameters of an instruction line
 * paremeters                : line       - a pointer to a parsed line struct
 * 							   symtable_p - a pointer to a symbol table
 * return                    : NO_ERROR - if all parameters are valid
 * 							   SYNTAX_ERROR - if commas in wrong places
 * 							   INVALID_NUM_OF_PARAMS - if the number of parameters doesnt
 * 							   						   match the operation
 * 							   INVALID_PARAMETERS - if the parameters are invalid*/
static error_value parse_instruction_params(parsed_line *line, symtable *symtable_p) {
	error_value err_val = NO_ERROR;
	int 	    len = strlen(line->parameters),
				num_of_params = get_num_of_params(line);
	char 		*param,
			    param_cpy[MAX_LINE_LEN];

	/*make a copy of the sting to work on so we dont alter the original string*/
	strcpy(param_cpy, line->parameters);

	/*if there are any parameters*/
	if (len) {
		/*check the commas in the parameters string*/
		if (param_cpy[0] == ',' || param_cpy[len - 1] == ',' || is_adjacent_commas(param_cpy)) {
			err_val = SYNTAX_ERROR;

			/*check if the number of parameters matches the operation*/
		} else if (num_of_params != get_op_num_of_params(line->name))
			err_val = INVALID_NUM_OF_PARAMS;
		else {

			/*check if the first parametes is legal*/
			param = strtok(param_cpy, PARSING_PARAMS_TOKENS);
			err_val = is_valid_instruction_param(param, symtable_p) ?
					NO_ERROR : INVALID_PARAMETERS;

			/*if theres a second parametes check if it is legal*/
			if (!err_val && num_of_params > 1) {
				param = strtok(NULL, PARSING_PARAMS_TOKENS);
				err_val = is_valid_instruction_param(param, symtable_p) ?
						NO_ERROR : INVALID_PARAMETERS;
			}
		}

		/*if no parameters then check if its an operation that doesnt require parameters*/
	} else if (strcmp(line->name, "stop") && strcmp(line->name, "rts"))
		err_val = INVALID_NUM_OF_PARAMS;

	return err_val;
}

/* parse_macro : a function to parse a macro line
 * parameters  : line       - a pointer to a parsed line struct
 * 				 token      - the current parsing token from strok
 * 				 symtable_p - a pointer to a symbol table
 * return      : NO_ERROR      - if line parsed succesfully
 * 			     NOT_A_NUMBER  - if the macro parameter is no a number
 * 			     INVALID_MACRO - if the macro name is invalid
 * 			     SYNTAX_ERROR  - if theres a syntax error in the line
 * 			     RESERVED_WORD - if the macro is a reserved word*/
static error_value parse_macro(parsed_line *line, char *token, symtable *symtable_p) {
	error_value err_val = NO_ERROR;
	char  *macro_name,
	      *macro_param;

	/*try to get the rest of the line, find the '=' sign and check if the
	 * syntax is correct*/
	if((macro_name = strtok(NULL, "\n\r")) &&
	   (macro_param = find_eq_sign(macro_name)) &&
       (macro_name != macro_param && macro_param + 1)){
		/* if everything if correct then save the name of the macro and the parameter*/
		macro_param[0] = '\0';
		macro_param++;
		/*parse the name of the macro and the parameter*/
		if(!(err_val = parse_macro_name(line, macro_name, symtable_p)))
			if(!(err_val = parse_macro_param(line, macro_param)))
				line->type = MACRO_TYPE;
	}else
		err_val = SYNTAX_ERROR;

	return err_val;
}

/* get_num_of_params : get the number of comma seperated parameters in the line
 * parameters        : line - a pointer to a parsed line struct
 * return 	         : the number of parameters*/
int get_num_of_params(parsed_line *line) {
	int  param_cnt;
	char *temp,
		 line_cpy[MAX_LINE_LEN];

	/*make a copy of the line so we wont alter the original line*/
	strcpy(line_cpy, line->parameters);

	/*tokenize and count the parameters */
	for (param_cnt = 0, temp = strtok(line_cpy, ",");
			temp; temp = strtok(NULL, ","), param_cnt++);

	return param_cnt;
}

/* parse_line : parse a raw line from the file into a struct containig a label, a
 * 			   name of operation, directive or macro and the parameters
 * parameters : line       - a pointer to a parsed line struct
 * 			    symtable_p - a pointer to a symbol table
 * return     : NO_ERROR              - if the line parsed succesfully
 *              INVAlID_LABEL         - if the label is invalid
 *              LABEL_TOO_LONG        - if the label is too long
 *              RESERVED_WORD         - if a reserved word is used as a label or parameter
 *              DUPLICATE_LABEL       - if a label that have already been declared is declared again
 *              NOT_A_NUMBER          - if a macro parameter is not a number
 *              INVALID_MACRO         - if a macro name is invalid
 *              SYNTAX_ERROR          - if there is a syntax error
 *              INVALID_DIRECTIVE     - if a directive is invalid
 *              INVALID_INSTRUCTION   - if an instruction is invalid
 *              MACRO_AFTER_LABEL     - if a macro declared in same line as label
 *              EMPTY_LABEL           - is a label is declared and the rest of the line is empty
 *              INVALID_PARAMETERS    - if the parameters of instruction are invalid
 *              INVALID_STRING        - if a string to a directive is invalid
 *              INVALID_NUM_OF_PARAMS - if the number of parameters to an instruction doesnt match the operation
 *
 *              */
error_value parse_line(parsed_line *line, symtable *symtable_p) {
	error_value err_val = NO_ERROR;
	int         token_num;
	char        *token;

	/*tokenize the line and parse each token*/
	for (token_num = 1, token = strtok(line->line, PARSING_WHITESPACE_TOKENS);
		 token && !err_val;
		 token = strtok(NULL, PARSING_WHITESPACE_TOKENS), token_num++) {

		/*parse each token by its location
		 * first token can be only label, define, a directive or an instruction
		 * second token can be only in instruction, a directive or parameters
		 * third token can be only parameters*/
		switch (token_num) {
		case 1:
			/*check if first token is label*/
			if (token[strlen(token) - 1] == ':') {
				/*if it is then remove the ':' at the end*/
				token[strlen(token) - 1] = '\0';
				/*check if the label is valid*/
				if (!(err_val = valid_label(token, symtable_p))) {
					/*if it is then check if its already previously declared*/
					if (!find_symbol(token, symtable_p))
						/*if not then add it to the parsed line struct*/
						strcpy(line->label, token);
					/*if yes then throw an error*/
					else
						err_val = DUPLICATE_LABEL;
				}
				/*check if first token is macro*/
			} else if (!strcmp(token, ".define"))
				/*if yes then parse it*/
				err_val = parse_macro(line, token, symtable_p);
			/*check if the first token is a directive*/
			else if (token[0] == '.') {
				/*if yes the parse it*/
				if (!(err_val = valid_directive(token))) {
					strcpy(line->name, token);
					line->type = DIRECTIVE_TYPE;
				}
				/*check if first token is an instruction*/
			} else if (!(err_val = valid_instruction(token))) {
				/*if yes then parse it*/
				strcpy(line->name, token);
				line->type = INSTRUCTION_TYPE;
			}
			break;
		case 2:
			/*check if first token was a label*/
			if (line->type == UNDEF_TYPE) {
				/*if yes then check if the second token in is a macro*/
				if (strcmp(token, ".define")){
					/*if not the check if second token is a directive*/
					if (token[0] == '.') {
						/*if yes then parse it*/
						if (!(err_val = valid_directive(token))) {
							strcpy(line->name, token);
							line->type = DIRECTIVE_TYPE;
						}
						/*if not then the second token must be an instruction*/
					} else if (!(err_val = valid_instruction(token))) {
						strcpy(line->name, token);
						line->type = INSTRUCTION_TYPE;
					}
					/*if its is a macro the throw an error because macro and label cant
					 * be defined in same line*/
				}else
					err_val = MACRO_AFTER_LABEL;
				/*if the first token wasnt a label then the second token can be only a parameter*/
			} else
				strcat(line->parameters, token);
			break;
			/*every token after the second must be a parameter so add it to the parameters
			 * of the parsed line struct*/
		default:
			strcat(line->parameters, token);
			break;
		}
	}

	/*if there wasnt an error in parsig the line then parse the parameters*/
	if(!err_val){
		/*if there was only a label then throw an empty label error*/
		if(token_num == 2 && line->type == UNDEF_TYPE)
			err_val = EMPTY_LABEL;
		else{
			/*parse the parameters of the line*/
			if (line->type == DIRECTIVE_TYPE)
				err_val = parse_directive_params(line, symtable_p);
			if (line->type == INSTRUCTION_TYPE)
				err_val = parse_instruction_params(line, symtable_p);
		}
	}

	return err_val;
}

/* reset parsed_line : reset the parsed line struct to fresh state
 * parameters        : line - a pointer to a parsed line struct
 * return            :
 */
void reset_parsed_line(parsed_line *line) {
	strcpy(line->label, "");
	strcpy(line->name, "");
	strcpy(line->line, "");
	strcpy(line->parameters, "");
	line->type = UNDEF_TYPE;
	line->macro_value = 0;
}

/* parse_macro_name : a function to parse the name of a macro
 * parameters       : line       - a pointer to a parsed line struct
 * 				      macro_name - the macro name string to parse
 * 				      symtable_p - a pointer to a symbol table
 * return           : NO_ERROR        - if parsed succesfully
 * 					  RESERVED_WORD   - if the name is a reserved word
 * 					  INVALID_MACRO   - if the macro name is invalid
 * 					  MACRO_TOO_LONG  - if the macro name is too long
 * 					  DUPLICATE_MACRO - if the macro name is already declared*/
error_value parse_macro_name(parsed_line *line, char *macro_name, symtable *symtable_p){
	error_value err_val = NO_ERROR;

	/*remove white space at teh begginning of the string*/
	if(isspace(macro_name[0]))
		macro_name = strtok(macro_name, PARSING_WHITESPACE_TOKENS);

	/*remove white space at the end*/
	macro_name = strtok(macro_name, PARSING_WHITESPACE_TOKENS);

	/*check if there still a string to parse and if its the only macro name in the string*/
	if(macro_name && !strtok(NULL, PARSING_WHITESPACE_TOKENS)){
		/*check if the name of the macro is valid*/
		if(!(err_val = valid_macro_name(macro_name, symtable_p))){
			/*check if the name is a reserved word*/
			if(!is_reserved_word(macro_name))
				/*add it to the parsed line struct*/
				strcpy(line->name, macro_name);
			else
				err_val = RESERVED_WORD;
		}
	}else
		err_val = SYNTAX_ERROR;

	return err_val;
}

/* parse_macro_paras : a function to parse the parameter of a macro
 * parameters        : line        - a pointer to a parsed line struct
 * 					   macro_param - the string o parameters to parse
 * return            : NO_ERROR     - if parsed succesfully
 * 					   NOT_A_NUMBER - if the parameter is no a number
 * 					   SYNTAX_ERROR - if a syntax error occures*/
error_value parse_macro_param(parsed_line *line, char *macro_param){
	error_value err_val = NO_ERROR;

	/*remove white space at the begginning*/
	if(isspace(macro_param[0]))
		macro_param = strtok(macro_param, PARSING_WHITESPACE_TOKENS);

	/*check if only one parameter is in teh string*/
	if(macro_param && !strtok(NULL, PARSING_WHITESPACE_TOKENS))
		/*check if its a legal number */
		if(is_legal_number(macro_param))
			/*if yes then add it to the parsed line struct*/
			line->macro_value = atoi(macro_param);
		else
			err_val = NOT_A_NUMBER;
	else
		err_val = SYNTAX_ERROR;

	return err_val;

}
