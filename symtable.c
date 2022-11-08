#include "symtable.h"

/* symtable_init : allocate and initialize a symbol table
 * parameters    :
 * return        : if succesfuly allocated the return a pointer to a symbol table
 * 				   else return NULL*/
symtable *symtable_init() {
	symtable *symtable_p;

	/*try to allocate memory and initialize a symbol table*/
	if ((symtable_p = malloc(sizeof(symtable)))) {
		symtable_p->table_size = 1;
		symtable_p->entry_flag = FALSE;
		symtable_p->symtable_entries = malloc(sizeof(symtable_entry));
	}

	return symtable_p;
}

/* symtable_free : free a previously allocated symbol table
 * parameters    : symtable_p - a pointer to a symbol table to free
 * return        :
 */
void symtable_free(symtable *symtable_p) {
	free(symtable_p->symtable_entries);
	free(symtable_p);
}

/* add_symbol : add a symbol to the symbol table
 * parameters : symtable_p - a pointer to a symbol table
 * 				name  - the name of the symbol to add
 * 				value - the value of the symbol
 * 				type  - the type of the symbol
 * return     : NO_ERROR           - if no error occured
 * 				ERROR_MEMORY_ALLOC - if a memory allocation error occured*/
error_value add_symbol(symtable *symtable_p, const char *name, const int value,
					   symbol_type type) {
	error_value err_val = NO_ERROR;

	/*if it is not the first entry increase the table size by one*/
	if(symtable_p->table_size > 1)
		symtable_p->symtable_entries = realloc(symtable_p->symtable_entries,
				(symtable_p->table_size) * sizeof(symtable_entry));

	/*if allocated succesfully then update its variables */
	if (symtable_p->symtable_entries) {
		strcpy(symtable_p->symtable_entries[symtable_p->table_size - 1].name,
				name);
		symtable_p->symtable_entries[symtable_p->table_size - 1].type = type;
		symtable_p->symtable_entries[symtable_p->table_size - 1].value = value;
		symtable_p->table_size++;
	} else
		err_val = ERROR_MEMORY_ALLOC;

	return err_val;
}

/* find_symbol : find a sybol by name in a symbol table
 * parameters  : name       - the name of the symbol to find
 * 				 symtable_p - a pointer to a symbol table
 * return      : if a symbol if found return a pointer to it
 *				 else return NULL */
symtable_entry *find_symbol(const char *name, symtable *symtable_p) {
	int i = -1;

	/*check if the symbol table is empty */
	if (symtable_p->symtable_entries)
		/*if no then itterate over it and look for the name of the symbol*/
		for (i = 0;
			 i < symtable_p->table_size &&
			 strcmp(name, symtable_p->symtable_entries[i].name);
			 i++);

	return (i < 0 || i == symtable_p->table_size) ?
			NULL : &symtable_p->symtable_entries[i];
}

/* find_macro : find a macro by name in the symbol table
 * paraeters  : name       - the name of the macro
 * 				symtable_p - a pointer to a symbol table
 * return     : if a macro is found return a pointer to it
 * 				else return NULL */
symtable_entry *find_macro(const char *name, symtable *symtable_p) {
	int i = -1;

	/*check if the symbol table is empty */
	if (symtable_p->symtable_entries)
		/*if no then itterate over it and look for the name of the macro*/
		for (i = 0;
			 i < symtable_p->table_size &&
			 (strcmp(symtable_p->symtable_entries[i].name, name) &&
			 symtable_p->symtable_entries[i].type != MACRO);
			 i++);

	return (i < 0 || i == symtable_p->table_size) ?
			NULL : &symtable_p->symtable_entries[i];
}

/* update_data_sym_values : update the data symbol to their new address after the first pass
 * parameters             : symtable_p - a pointer to a symbol table
 * 						    ic - the ic of to update the data value by
 * return                 :*/
void update_data_sym_values(symtable *symtable_p, const int ic) {
	int i;

	/*check if the symbol table is empty */
	if(symtable_p->symtable_entries)
		/*if no the itterate over it and update every data value*/
		for (i = 0; i < symtable_p->table_size; i++) {
			if (symtable_p->symtable_entries[i].type == DATA)
				symtable_p->symtable_entries[i].value += (ADDRESS_OFFSET + ic);
		}
}
