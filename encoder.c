#include "encoder.h"
#include "utils.h"

/*the base we encode in*/
#define BASE_SIZE 4
/*a mask to held encoding the words*/
#define BASE_MASK 3
/*the size of the encoded word*/
#define SPECIAL_BASE_WORD_SIZE 7
/*amount to shift the encoding word*/
#define BASE_MOVE 2

/*enum for the value of the addressing modes*/
typedef enum{
		IMMEDIATE_VAL,
		DIRECT_VAL,
		INDEX_VAL,
		REGISTER_VAL
} addressing_mode_value;

/*enum of the values of the coding modes*/
typedef enum{
	ABS = 0,
	RELOC = 2,
	EXT = 1
}coding_mode;

/*a table of the special 4 base characters */
static char special_base_table[BASE_SIZE + 1] = "*#%!";

/* encode_immediate : a function to encode a parameter with IMMEDIATE addressign mode
 * parameters       : mem_img    - a poitner to a memory image
 *                    symtable_p - a pointer to a symbol table
 *                    token      - the parameter to encode
 * return           :*/
static void encode_immediate(memory_image *mem_img, symtable *symtable_p, const char *token){
	int value = 0,
		word = 0;

	/*if the parameter is a number of a macro get the value to encode accordingly*/
	value = isalpha(token[1]) ?
			find_symbol(token + 1, symtable_p)->value : atoi(token + 1);
	/*encode the value and the coding mode*/
	word = word | encode_dest_mode(value);
	word = word | ABS;

	/*update the reserved space in the code table to the encoded word*/
	mem_img->code->code_entries[mem_img->code->ic].bin_machine_code = word;
	mem_img->code->ic++;
}

/* encode_direct : a function to encode and add a parameter with DIRECT addressing mode
 * 				   to the code table
 * parameters    : mem_img    - a pointer to a memory image
 * 			       symtable_p - a pointer to a symbol table
 * 			       token      - the parameter to encode
 * return        : NO_ERROR    - if no error occured
 * 				   LABEL_UNDEF - if a parameter is an undefined label*/
static error_value encode_direct(memory_image *mem_img, symtable *symtable_p, const char *token){
	error_value    err_val = NO_ERROR;
	symtable_entry *sym;
	coding_mode    c_mode;
	int            value = 0,
				   word = 0;

	/*check if the label is defined*/
	if (!(sym = find_symbol(token, symtable_p)))
		err_val = LABEL_UNDEF;

	/*if the label is defined*/
	if (!err_val) {
		/*check if the label is external*/
		if (sym->type == EXTERNAL) {
			/*if its external the flag it as external and encode it a such*/
			c_mode = EXT;
			strcpy(mem_img->code->code_entries[mem_img->code->ic].extern_name,
					token);
			mem_img->code->extern_flag = TRUE;
		} else {
			/*if its not external get its value */
			value = sym->value;
			c_mode = RELOC;
		}

		/*encode the value of label and its coding mode */
		word = word | encode_dest_mode(value);
		word = word | c_mode;

		/*update the reserved word in the code table to the enoded value*/
		mem_img->code->code_entries[mem_img->code->ic].bin_machine_code = word;
		strcpy(mem_img->code->code_entries[mem_img->code->ic].extern_name,
			   token);
		mem_img->code->ic++;
	}

	return err_val;
}

/* encode_index : encode and add a parameter with INDEX addressing mode to the code table
 * parameters   : mem_img    - a pointer to a memory image
 * 		   	      symtable_p - a pointer to a symbol table
 * 		   	      token      - the parameter to encode
 * return       : NO_ERRRO    - if no error occured
 * 				  LABEL_UNDEF - if a label in the parameters is undefined  */
static error_value encode_index(memory_image *mem_img, symtable *symtable_p, const char *token){
	error_value    err_val = NO_ERROR;
	char 		   arr[MAX_LABEL_LEN];
	symtable_entry *sym;
	coding_mode    c_mode;
	int 		   word = 0,
				   value = 0;

	/*get the name of the array*/
	get_arr_name(token, arr);

	/*check if its is defined */
	if (!(sym = find_symbol(arr, symtable_p)))
		err_val = LABEL_UNDEF;

	/*if the name is defined*/
	if (!err_val) {
		/*check if its external*/
		if (sym->type == EXTERNAL) {
			c_mode = EXT;
			strcpy(mem_img->code->code_entries[mem_img->code->ic].extern_name,
					token);
			mem_img->code->extern_flag = TRUE;
		} else {
			value = sym->value;
			c_mode = RELOC;
		}

		/*encode the value of the array name*/
		word = word | encode_dest_mode(value);
		word = word | c_mode;

		/*update the encoded value of the name to the code table*/
		mem_img->code->code_entries[mem_img->code->ic].bin_machine_code = word;
		strcpy(mem_img->code->code_entries[mem_img->code->ic].extern_name,
				token);
		mem_img->code->ic++;

		word = 0;

		/*get the index of the array*/
		get_arr_index(token, arr);

		/*check if its a number or a macro and get the value accordingly*/
		value = isalpha(arr[0]) ?
				find_symbol(arr, symtable_p)->value : atoi(arr);

		/*encode the value in the word*/
		word = word | encode_dest_mode(value);
		word = word | ABS;

		/*update the reserverd code word in the code table with the encoded value*/
		mem_img->code->code_entries[mem_img->code->ic].bin_machine_code = word;
		mem_img->code->ic++;
	}

	return err_val;
}

/* encode_register : a functio to encode and add a parameter with REGISTER addressing mode
 *                   to the code table
 * parameters      : mem_img  - a pointer to a memory image
 * 					 token    - the parameter to encode
 * 					 reg_flag - a flag if a register was encoded in the previous line
 * return          : */
static void encode_register(memory_image *mem_img, const char *token, const int reg_flag, const int num_of_param){
	int word = 0;

	/*encoded by the number of the parameter*/
	switch (num_of_param) {
	/*if its the first parameter encode normaly*/
	case 1:
		/*encode the value of the register */
		word = word | encode_src_reg(get_reg_val(token));
		word = word | ABS;

		/*update the reserved code word with the endoed value*/
		mem_img->code->code_entries[mem_img->code->ic].bin_machine_code = word;
		mem_img->code->ic++;
		break;
	case 2:
		/*if its the second parameter check if the first was a register */
		if (reg_flag)
			/*if it was then encode the previous word in the code table*/
			mem_img->code->ic--;

		/*if not the encode normaly*/
		mem_img->code->code_entries[mem_img->code->ic].bin_machine_code =
				mem_img->code->code_entries[mem_img->code->ic].bin_machine_code |
				encode_dest_reg(get_reg_val(token));

		mem_img->code->ic++;
		break;
	}
}

/* word_to_4_special_base : a function to translate from binary to the special 4
 * 				            base of the assembler
 * parameters             : */
void word_to_4_special_base(int word, char *special_base) {
	int mask = BASE_MASK,
		i;


	special_base[SPECIAL_BASE_WORD_SIZE] = '\0';

	/*the index of the special character in the special 4 table is its value
	 * so we get the value of the encoded binary word and translate every 2 digits
	 * to its representative special character in base 4*/
	for (i = SPECIAL_BASE_WORD_SIZE - 1; i >= 0; i--) {
		special_base[i] = special_base_table[word & mask];

		/*we shift the word to encoded all the word*/
		word >>= BASE_MOVE;
	}
}

/* encode_instruction : a function to finish encoding the instruciton line
 * 						after we encoded the first word in the first pass
 * parameters         : token      - the parameters of the line
 * 						symtable_p - a pointer to a symbol table
 * 				        mem_img    - a pointer to a memory image
 * return             : NO_ERROR    - if no error occured
 * 						LABEL_UNDEF - if a parameter is an undefined label */
error_value encode_instruction(char *token, symtable *symtable_p,
							   memory_image *mem_img) {
	error_value     err_val = NO_ERROR;
	int   			i,
					reg_flag = FALSE,
					op_params = get_op_num_of_params(token);
	addressing_mode addr_mode;

	/*itterate every parameter of the line*/
	for (i = 0, mem_img->code->ic++; !err_val && i < op_params; i++) {
		token = strtok(NULL, PARSING_PARAMS_TOKENS);
		/*get the addressinf mode of the parameter*/
		addr_mode = get_addr_mode(token, symtable_p);

		switch (addr_mode) {
		/*if the parameters addressing mode is IMMEDIATE*/
		case IMMEDIATE:
			encode_immediate(mem_img, symtable_p, token);
			break;
			/*if the parameters addressing mode is DIRECT*/
		case DIRECT:
			err_val = encode_direct(mem_img, symtable_p, token);
			break;
			/*if the parameters addressing mode is INDEX*/
		case INDEX:
			err_val = encode_index(mem_img, symtable_p, token);
			break;
			/*if the parameters addressing mode is REGISTER*/
		case REGISTER:
			encode_register(mem_img, token, reg_flag, i + 1);
			reg_flag = TRUE;
			break;
		default:
			err_val = SYNTAX_ERROR;
		}
	}

	return err_val;
}

/* get_addr_mode : a function to get the addressing mode of a parameter
 * parameters    : param      - the parameter to get its addressing mode
 * 				   symtable_p - a pointer to a symbol table
 * return        : the parameters addresing mode*/
int get_addr_mode(char *param, symtable *symtable_p) {
	addressing_mode mode;

	if (param[0] == '#')
		mode = IMMEDIATE;
	else if (!valid_label(param, symtable_p))
		mode = DIRECT;
	else if (get_reg_val(param) >= 0)
		mode = REGISTER;
	else
		mode = INDEX;

	return mode;
}

/* get_addt_mode_val : a function to get the value of an addressing mode
 * parameters        : mode - the mode to get its value
 * return            : the value of the addressing mode */
int get_addr_mode_val(int mode) {
	int ret_val;

	switch (mode) {
	case IMMEDIATE:
		ret_val = IMMEDIATE_VAL;
		break;
	case DIRECT:
		ret_val = DIRECT_VAL;
		break;
	case INDEX:
		ret_val = INDEX_VAL;
		break;
	case REGISTER:
		ret_val = REGISTER_VAL;
		break;
	default:
		ret_val = -1;
	}

	return ret_val;
}
