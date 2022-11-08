#ifndef SYMTABLE_H
#define SYMTABLE_H

#include "defs.h"
#include "error.h"
#include "symtable.h"

/*enum for the types of symbols*/
typedef enum symbol_types {
	DATA, CODE, EXTERNAL, MACRO, ENTRY
} symbol_type;

/*a struct of a symbol table entry*/
typedef struct{
	char name[MAX_LABEL_LEN];
	int value;
	symbol_type type;
} symtable_entry;

/*a struct representing a symbol table*/
typedef struct{
	int table_size;
	symtable_entry *symtable_entries;
	int entry_flag;
} symtable;

symtable *symtable_init();
void symtable_free(symtable*);
symtable_entry *find_symbol(const char*, symtable*);
symtable_entry *find_macro(const char*, symtable*);
void update_data_sym_values(symtable*, const int);
error_value add_symbol(symtable*, const char*, const int, symbol_type);

#endif
