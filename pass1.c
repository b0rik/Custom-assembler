#include "pass1.h"
#include "parser.h"
#include "encoder.h"
#include "utils.h"

/*pass1_handle_directive : a function to handle a directive line
 *parameters             : line       - a pointer to a parsed line struct
 *                         symtable_p - a pointer to a symbol_table
 *                         mem_img    - a pointer to a memory image
 *return                 : NO_ERROR              - if no error occured
 *                         ERROR_MEMORY_ALLOC    - if encountered a memory allocation error
 *                         MACRO_PARAM_UNDEFINED - if an undefined macro parameter is passed*/
static error_value pass1_handle_directive(parsed_line *line, symtable *symtable_p,
										  memory_image *mem_img) {
	error_value err_val;

	/*if there is a label add it to the symbol table
	 * expect and entry line to extern then igone the label*/
	if (strlen(line->label) && strcmp(line->name, ".entry")
							&& strcmp(line->name, ".extern"))
		err_val = add_symbol(symtable_p, line->label, mem_img->data->dc, DATA);
	/*if label succesfully added*/
	if (!err_val) {
		/*if its a string directive then try to add the string to the data table*/
		if (!strcmp(line->name, ".string"))
			err_val = add_string_data(mem_img->data, line->parameters);
		/*if its a data directive the try to add the numbers to the data table*/
		else if (!strcmp(line->name, ".data"))
			err_val = add_num_data(mem_img->data, line->parameters, symtable_p);
		/*if its and extern directive then try to add it to the symbol table*/
		else if (!strcmp(line->name, ".extern"))
			err_val = add_symbol(symtable_p, line->parameters, 0, EXTERNAL);
	}

	return err_val;
}

/* pass1_handle_instruction : a funtion to handle an instruction line
 * parameters               : line       - a pointer to a parsed line struct
 * 							  symtable_p - a pointer to a symbol table
 * 							  mem_img    - a pointer to a memory image
 * return			        : NO_ERROR               - if no error occured
 * 							  ERROR_MEMORY_ALLOC     - if a memory allocation error occured
 * 							  INVALID_ADDR_DEST_MODE - if the parameters source addressing
 * 							                           mode is not allowed by the operation
 * 							  INVALID_ADDR_SRC_MODE  - if the parametersdestination addressing
 * 							  					       mode is not allowed by the operation*/
static error_value pass1_handle_instruction(parsed_line *line, symtable *symtable_p,
											memory_image *mem_img) {
	error_value     err_val = NO_ERROR;
	int			    word = 0,
					i,
					op_params = get_op_num_of_params(line->name),
					allowed_op_src = get_op_allowed_src(line->name),
					allowed_op_dest = get_op_allowed_dest(line->name),
					op_value = get_op_value(line->name),
					addr_mode[2];
	char 			*param;

	/*if the line has a label try to add it to the symbol table with the value
	 * of the current ic + the address offset we assume the program start from*/
	if (strlen(line->label))
		err_val = add_symbol(symtable_p, line->label,
				  mem_img->code->ic + ADDRESS_OFFSET, CODE);
	/*if label succesfully added*/
	if (!err_val) {
		/*encode the operation in the memory word*/
		word = word | encode_op(op_value);
		/*get the first parameter of the line if any*/
		param = strtok(line->parameters, PARSING_PARAMS_TOKENS);

		/*itterate over the parameters of the line and encode the first memory word
		 * by their addressing modes*/
		for (i = 0; !err_val && param && i < op_params;
			 i++, param = strtok(NULL, PARSING_PARAMS_TOKENS)) {
			addr_mode[i] = get_addr_mode(param, symtable_p);

			/*if the operation requires one parameter*/
			if (op_params == 1) {
				/*encode the only parameter*/
				if (i == 0 && !(err_val = allowed_op_dest & addr_mode[i] ?
										  err_val : INVALID_ADDR_DEST_MODE))
					word = word | encode_dest_mode(get_addr_mode_val(addr_mode[i]));
			} else {
				/*if the operation requires two parameters
				 * encode both to their appropriate location in the memory word*/
				if (i == 0 && !(err_val = allowed_op_src & addr_mode[i] ?
									      err_val : INVALID_ADDR_SRC_MODE))
					word = word | encode_src_mode(get_addr_mode_val(addr_mode[i]));
				else if (!(err_val = allowed_op_dest & addr_mode[i] ?
									 err_val : INVALID_ADDR_DEST_MODE))
					word = word | encode_dest_mode(get_addr_mode_val(addr_mode[i]));
			}
			/*if the operation reqires no parameters we wont itterate and wont encode the parameters*/
		}

		/*if encoding the first word was succesfull then reserve word in the code table
		 * for the parameters to encode in the second pass*/
		if (!err_val) {
			/*add the first word to the code table*/
			add_code(mem_img->code, word);
			for (i = 0; i < op_params; i++) {
				/*if the addresing mode is INDEX then reserve two word for each parametes
				 * one for the name of the label and the second for the index*/
				if (addr_mode[i] == INDEX) {
					add_code(mem_img->code, 0);
					add_code(mem_img->code, 0);
					/*for every other addresing mode reserve one word for every parameter*/
				} else {
					add_code(mem_img->code, 0);
					/*except if both parameres are REGISTER addressing mode then reserve
					 * one word for both*/
					if (i == 0 && addr_mode[i] == REGISTER
							   && addr_mode[i + 1] == REGISTER)
						break;
				}

			}
		}
	}

	return err_val;
}

/* pass1_handle_line : a function to handle a line in the first pass
 * parameter         : line       - a pointer to a parsed line struct
 * 				       symtable_p - a pointer to a symbol table
 * 				       mem_img    - a pointer to a memory image
 * return            : NO_ERROR               - if no error occured
 * 				       LINE_TOO_LONG          - if the line is too long
 * 				       INVAlID_LABEL          - if the label is invalid
 *             		   LABEL_TOO_LONG         - if the label is too long
 *              	   RESERVED_WORD          - if a reserved word is used as a label or parameter
 *              	   DUPLICATE_LABEL        - if a label that have already been declared is declared again
 *              	   NOT_A_NUMBER           - if a macro parameter is not a number
 *              	   INVALID_MACRO          - if a macro name is invalid
 *              	   SYNTAX_ERROR           - if there is a syntax error
 *              	   INVALID DIRECTIVE      - if a directive is invalid
 *              	   INVALID_INSTRUCTION    - if an instruction is invalid
 *              	   MACRO_AFTER_LABEL      - if a macro declared in same line as label
 *              	   EMPTY_LABEL            - is a label is declared and the rest of the line is empty
 *              	   INVALID_PARAMETERS     - if the parameters of instruction are invalid
 *              	   INVALID_STRING         - if a string to a directive is invalid
 *              	   INVALID_NUM_OF_PARAMS  - if the number of parameters to an instruction doesnt match the operation
 *              	   ERROR_MEMORY_ALLOC     - if encountered a memory allocation error
 *                     MACRO_PARAM_UNDEFINED  - if an undefined macro parameter is passed
 *                     INVALID_ADDR_DEST_MODE - if the parameters source addressing
 * 							                    mode is not allowed by the operation
 * 					   INVALID_ADDR_SRC_MODE  - if the parametersdestination addressing
 * 							  				   mode is not allowed by the operation*/
static error_value pass1_handle_line(parsed_line *line, symtable *symtable_p,
							  memory_image *mem_img) {
	error_value err_val = NO_ERROR;

	/*check if the line is valid*/
	if (!(err_val = line_valid(line->line)))
		/*if yes then check if its an empty line or a comment line*/
		if (!is_empty_line(line->line) && !is_comment_line(line->line))
			/*if no then parse the line*/
			if (!(err_val = parse_line(line, symtable_p)))
				/*handle each line by its type*/
				switch (line->type) {
				/*if the line is a macro line the add the macro to the symbol table*/
				case MACRO_TYPE:
					err_val = add_symbol(symtable_p, line->name, line->macro_value,
							   MACRO);
					break;
					/*if its a directive line then encode it and add the data to the data table*/
				case DIRECTIVE_TYPE:
					err_val = pass1_handle_directive(line, symtable_p, mem_img);
					break;
					/*if its an instruction line then encode the first word and reserve
					 * words for the second pass in the code table*/
				case INSTRUCTION_TYPE:
					err_val = pass1_handle_instruction(line, symtable_p,
													   mem_img);
					break;
				default:
					err_val = SYNTAX_ERROR;
				}

	return err_val;
}

/* pass1_execute : a function that executes the first pass
 * parameters    : fp         - a pointer to a file
 * 				   mem_img_p  - a pointer to a memory image
 * 				   symtable_p - a pointer to a symbol table
 * 				   file_name  - the name of the file currently proccesed
 * return        : NO_ERROR    - if no error occured
 * 				   ERROR_PASS1 - if there was an error in the first pass*/
error_value pass1_execute(FILE *fp, memory_image *mem_img_p,
						  symtable *symtable_p, const char *file_name) {
	error_value err_val = NO_ERROR;
	int 		err_flag = FALSE,
				line_cnt;
	parsed_line *line;

	/*allocate a parsed lin struct*/
	if((line = malloc(sizeof(parsed_line)))){
		/*itterate every line of the file and parse it*/
		for (line_cnt = 1, reset_parsed_line(line);
			 fgets(line->line, MAX_LINE_LEN + 2, fp);
			 line_cnt++, reset_parsed_line(line)) {
			/*execute first pass of the line*/
			if ((err_val = pass1_handle_line(line, symtable_p, mem_img_p))) {
				err_flag = TRUE;
				print_error(err_val, file_name, line_cnt);
			}
		}
		free(line);
	} else
		print_error(err_val = ERROR_MEMORY_ALLOC, file_name, 0);

	return err_flag ? ERROR_PASS1 : NO_ERROR;
}
