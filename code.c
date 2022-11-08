#include "code.h"
#include "error.h"

/*the number of operations supported by the cpu*/
#define NUM_OF_OPS 16
/*the nummber of registers*/
#define NUM_OF_REGISTERS 8
/*max lenght of operation name*/
#define MAX_OP_LEN 4
/*max lenght of register name*/
#define MAX_REG_LEN 2

/*a struct representing an operation supported by the cpu*/
typedef struct {
	char name[MAX_OP_LEN + 1];
	int value;
	int num_of_parameters;
	int allowed_src_mode;
	int allowed_dest_mode;
} op_entry;

/*a struct representing a register of the cpu*/
typedef struct {
	char name[MAX_REG_LEN + 1];
	int value;
} register_entry;

/* a table of operations supported by the cpu with their name, value, number of parameters
 * and allowed source and estination addressing modes while each mode turnes a different
 * bit for bitwise operators to use
 * for example if an operation supports the DIRECT source addressing mode
 * the command mode & op->allowed_src_mode will give a non zero result
 * which means it supports this mode*/
static op_entry op_table[NUM_OF_OPS] = {
		{"mov" ,  0, 2, IMMEDIATE | DIRECT | INDEX |  REGISTER, DIRECT | INDEX |  REGISTER},
		{"cmp" ,  1, 2, IMMEDIATE | DIRECT | INDEX |  REGISTER, IMMEDIATE | DIRECT | INDEX |  REGISTER},
		{"add" ,  2, 2, IMMEDIATE | DIRECT | INDEX |  REGISTER, DIRECT | INDEX |  REGISTER},
		{"sub" ,  3, 2, IMMEDIATE | DIRECT | INDEX |  REGISTER, DIRECT | INDEX |  REGISTER},
		{"not" ,  4, 1, NONE, DIRECT | INDEX |  REGISTER},
		{"clr" ,  5, 1, NONE, DIRECT | INDEX |  REGISTER},
		{"lea" ,  6, 2, DIRECT | INDEX, DIRECT | INDEX |  REGISTER},
		{"inc" ,  7, 1, NONE, DIRECT | INDEX |  REGISTER},
		{"dec" ,  8, 1, NONE, DIRECT | INDEX |  REGISTER},
		{"jmp" ,  9, 1, NONE, DIRECT | REGISTER},
		{"bne" , 10, 1, NONE, DIRECT | REGISTER},
		{"red" , 11, 1, NONE, DIRECT | INDEX |  REGISTER},
		{"prn" , 12, 1, NONE, IMMEDIATE | DIRECT | INDEX |  REGISTER},
		{"jsr" , 13, 1, NONE, DIRECT | REGISTER},
		{"rts" , 14, 0, NONE, NONE},
		{"stop", 15, 0, NONE, NONE}
};

/*a table of the registers of the cpu*/
static register_entry registers[NUM_OF_REGISTERS] = {
		{"r0", 0},
		{"r1", 1},
		{"r2", 2},
		{"r3", 3},
		{"r4", 4},
		{"r5", 5},
		{"r6", 6},
		{"r7", 7}
};

/* get_op : the function receives a name and returns the operation that
 * 					 has this name and if no such operation exists then NULL is returned
 *
 * parameters      : op_name - a of and operation
 * return          : if the operation exists in the table the return a pointer to it
 * 				     else returns NULL
 * */
static op_entry *get_op(const char* op_name) {
	op_entry *p;
	int 	 i;

	/* itterate over the operations table with a pointer and compare the received name
	 * with the name of the operations in the table and stop when a same name is
	 * found*/
	for (i = 0, p = op_table; i < NUM_OF_OPS; i++, p++)
		if (!strcmp(p->name, op_name))
			break;

	/*return the pointer or NULL if not found a same name*/
	return i != NUM_OF_OPS ? p : NULL;
}

/* get_register : the function receives a name and returns the register that
 * 				  has this name and if no such register exists then NULL is returned
 *
 * parameters   : reg_name - a of and operation
 * return       : if the operation exists in the table the return a pointer to it
 * 				  else returns NULL
 * */
static register_entry *get_register(const char *reg_name) {
	int 		   i;
	register_entry *reg_entry;

    /* itterate over the register table with a pointer and compare the received name
     * with the name of the registers in the table and stop when a same name is
	 * found*/
	for (i = 0, reg_entry = registers; i < NUM_OF_REGISTERS; i++, reg_entry++)
		if (!strcmp(reg_name, reg_entry->name))
			break;

	/*return the pointer or NULL if not found a same name*/
	return i != NUM_OF_REGISTERS ? reg_entry : NULL;
}

/* code_table_init : a function to allcate memory for the code talbe and initialize it
 * parameters      :
 * return          : if initialized succsessfully the return a pointer to the initialized
 * 					 table
 * 					 else return NULL
 */
code_table *code_table_init() {
	code_table *code_table_p;

	/*allocate memory and check if succsessfully allocated
	 * then initialize the variables of the table*/
	if ((code_table_p = malloc(sizeof(code_table)))) {
		code_table_p->extern_flag = FALSE;
		code_table_p->ic = 0;
		code_table_p->code_entries = malloc(sizeof(code_entry));
	}

	/*return the pointer to the table or NULL if allocation failed*/
	return code_table_p;
}
/* code_table_free : free the previously allocated code_table
 * parameters     :
 * return         :
 */
void code_table_free(code_table *code_table_p) {
	free(code_table_p->code_entries);
	free(code_table_p);
}

/* add_code   : a function to add a code entry to the code table
 * parameters : code_table_p - a pointer to a code_table
 * 				value - the value we want to add to the code table
 * return     : NO_ERROR		   - if successfully added the code entry
 * 				ERROR_MEMORY_ALLOC - if there was an error allocatin memory for the
 * 				 	 				 new entry*/
error_value add_code(code_table *code_table_p, const int value) {
	error_value err_val = NO_ERROR;

	/*if it is not the first entry to the code table increase its size by one*/
	if(code_table_p->ic)
		code_table_p->code_entries = realloc(code_table_p->code_entries,
				(code_table_p->ic + 1) * sizeof(code_entry));

	/* if successfully allocated the table array
	 * set the ic of the code entry as the current table size + the address offset
	 * we assume the program starts at
	 * and set the value as the received parameter*/
	if (code_table_p) {
		code_table_p->code_entries[code_table_p->ic].address = code_table_p->ic
				+ ADDRESS_OFFSET;
		code_table_p->code_entries[code_table_p->ic].bin_machine_code = value;
		strcpy(code_table_p->code_entries[code_table_p->ic].extern_name, "");
		/*incremet the ic (we use it as a the size of the table too*/
		code_table_p->ic++;
	} else /*if allocation has failed*/
		err_val = ERROR_MEMORY_ALLOC;

	return err_val;
}

/* get_op_value : return the value of the provided operation name
 * 				  or -1 if it doesnt exists
 * parameters   : op_name - the name of the operation to get its value
 * return       : if operation exists return its value
 * 				  else return -1*/
int get_op_value(const char *op_name){
	op_entry *p;

	/*look for the operation in the table if exists return it value else return -1 */
	return ((p = get_op(op_name))) ? p->value : -1;
}

/* get_op_num_of_params : return the number of parameters of the provided
 * 						  operation name
 * 						  or -1 if it doesnt exists
 * parameters   		: op_name - the name of the operation to get its number of
 * 								    parameters
 * return      		    : if operation exists return its number of parameters
 * 						  else return -1*/
int get_op_num_of_params(const char *op_name){
	op_entry *p;

	/* look for the operation in the table if exists return it number of
	 * parameters else return -1 */
	return ((p = get_op(op_name))) ? p->num_of_parameters : -1;
}

/* get_op_allowed_src : return the allowed source addressing modes of the provided
 * 						operation name
 * 						or -1 if it doesnt exists
 * parameters   	  : op_name - the name of the operation to get its allowed
 * 								  source addressing modes
 * return      		  : if operation exists return its allowed source addressing modes
 * 						else return -1*/
int get_op_allowed_src(const char *op_name){
	op_entry *p;

	/* look for the operation in the table if exists return it allowed source
	 * addressing modes else return -1 */
	return ((p = get_op(op_name))) ? p->allowed_src_mode : -1;
}

/* get_op_allowed_dest : return the allowed destination addressing modes of the provided
 * 						 operation name
 * 					     or -1 if it doesnt exists
 * parameters   	   : op_name - the name of the operation to get its allowed
 * 							       destination addressing modes
 * return      		   : if operation exists return its allowed destination addressing modes
 * 						 else return -1*/
int get_op_allowed_dest(const char *op_name){
	op_entry *p;

	/* look for the operation in the table if exists return it allowed destination
     * addressing modes else return -1 */
	return ((p = get_op(op_name))) ? p->allowed_dest_mode : -1;
}

/* get_reg_val : return the value of the provided register name
 * 			     or -1 if it doesnt exists
 * parameters  : reg_name - the name of the register to get its value
 * return      : if register exists return its value
 * 			     else return -1*/
int get_reg_val(const char *reg_name){
	register_entry *reg_entry;

	/* look for the register in the table if exists return its value else return -1 */
	return ((reg_entry = get_register(reg_name))) ? reg_entry->value : -1;
}
