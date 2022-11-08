#include "data.h"
#include "symtable.h"
#include "error.h"

/*the number of directives*/
#define NUM_OF_DIRECTIVES 4
/*max length of a directives name*/
#define MAX_DIRECTIVE_LEN 8

/*a table of all the directives*/
static char directive_table[NUM_OF_DIRECTIVES][MAX_DIRECTIVE_LEN] = {
		"data",
		"string",
		"extern",
		"entry"
};

/* add_data   : add a data entry to the data table
 * parameters : data_table_p - a pointer to a data table
 * 				value 		 - the value to add to the table
 * return     : NO_ERRRO 	       - if data added succsessfuly
 * 				ERROR_MEMORY_ALLOC - if there was an error allocating the
 * 									 the new entry*/
static error_value add_data(data_table *data_table_p, const int value) {
	error_value err_val = NO_ERROR;

	/*if is not the first entry to the data table the increase its size by one*/
	if(data_table_p->dc)
		data_table_p->data_entries = realloc(data_table_p->data_entries,
				(data_table_p->dc + 1) * sizeof(data_entry));

	/* if successfully allocated the table array
	 * set the dc of the data entry as the current table size
	 * and set the value as the received parameter*/
	if (data_table_p) {
		data_table_p->data_entries[data_table_p->dc].address = data_table_p->dc;
		data_table_p->data_entries[data_table_p->dc].value = value;
		/*incremet the ic (we use it as a the size of the table too*/
		data_table_p->dc++;
	} else /*if allocation has failed*/
		err_val = ERROR_MEMORY_ALLOC;

	return err_val;
}

/* data_table_init : a function to allcate memory for the data talbe and initialize it
 * parameters      :
 * return          : if initialized succsessfully the return a pointer to the initialized
 * 					 table
 * 					 else return NULL
 */
data_table *data_table_init() {
	data_table *data_table_p;

	/*allocate memory and check if succsessfully allocated
	 * then initialize the variables of the table*/
	if ((data_table_p = malloc(sizeof(data_table)))){
		data_table_p->dc = 0;
		data_table_p->data_entries = malloc(sizeof(data_entry));
	}

	/*return the pointer to the table or NULL if allocation failed*/
	return data_table_p;
}

/* data_table_free : free the previously allocated data table
 * parameters     :
 * return         :
 */
void data_table_free(data_table *data_table_p) {
	free(data_table_p->data_entries);
	free(data_table_p);
}

/* add_string_data : add a string to the data table character by character
 * parameters      : data_table_p - a pointer to a data table
 * 					 str - the string to add to the table
 * return          : NO_ERROR           - if string added successfully
 * 				     ERROR_MEMORY_ALLOC - if a problem occured allocatin an
 * 				     					  entry to the table*/
error_value add_string_data(data_table *data_table_p, const char *str) {
	error_value err_val = NO_ERROR;
	int			i,
			    len = strlen(str);

	/*add every character from str*/
	for (i = 1; !err_val && i < len - 1; i++)
		err_val = add_data(data_table_p, str[i]);

	/*add 0 terminator*/
	if (!err_val)
		err_val = add_data(data_table_p, '\0');

	return err_val;
}

/* add_num_data : add numbers to the data table number by number
 * parameters   : data_table_p - a pointer to a data table
 * 				  params       - the string of numbers to add to the table
 * 				  symtable_p   - a pointer to a symbol table for macros
 * return       : NO_ERROR              - if string added successfully
 * 				  ERROR_MEMORY_ALLOC    - if a problem occured allocatin an
 * 				      		              entry to the table
 * 				  MACRO_PARAM_UNDEFINED - if an undefined macro has been passed as a number*/
error_value add_num_data(data_table *data_table_p, char *params,
						 symtable *symtable_p) {
	error_value    err_val = NO_ERROR;
	symtable_entry *sym_entry;
	int            data_value;
	/*get the first number*/
	char           *param = strtok(params, PARSING_DATA_NUM_PARAMS_TOKENS);

	/*get all the numbers and add them to the data table*/
	for (; !err_val && param;
			param = strtok(NULL, PARSING_DATA_NUM_PARAMS_TOKENS)) {
		/*check if it is a number*/
		if (isdigit(param[0]) || param[0] == '+' || param[0] == '-')
			data_value = atoi(param);
		/* if not a number then it is a macro, find it*/
		else if ((sym_entry = find_symbol(param, symtable_p)))
			data_value = sym_entry->value;
		/*if no such macro found*/
		else
			err_val = MACRO_PARAM_UNDEFINED;

		/*add the number of the found macro to the data table*/
		if (!err_val)
			err_val = add_data(data_table_p, data_value);
	}

	return err_val;
}

/* update_data_addr : update the address of all teh data entries by ic + the address offset
 * 			          we assume the program runs from
 * parameters       : data_table_p - a pointer to a data table
 * 					  ic           - the ic to add to the addresses
 * return 		    :
 */
void update_data_addr(data_table *data_table_p, const int ic) {
	int i;

	/*itterate over the data table and update the address of every entry*/
	if (data_table_p->data_entries) {
		for (i = 0; i < data_table_p->dc; i++)
			data_table_p->data_entries[i].address += (ADDRESS_OFFSET + ic);
	}
}

/* valid_directive : check if a string is a valid directive
 * parameters      : str - the string to check
 * return          :INVALID_DIRECTIVE - if the string is not a valid directive
 * 					NO_ERROR 		  - if the string is a valid directive*/
error_value valid_directive(const char *str){
	int 	   i;
	const char *p = str + 1;

	/*find the string in the directives table*/
	for(i = 0; strcmp(p, directive_table[i]) && i < NUM_OF_DIRECTIVES; i++);

	/*if found return NO ERROR else return INVALID_DIRECTIVE*/
	return i == NUM_OF_DIRECTIVES ? INVALID_DIRECTIVE : NO_ERROR;
}
