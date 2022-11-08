#ifndef CODE_H
#define CODE_H

#include "defs.h"
#include "error.h"

/*a struct representing an entry in the code table*/
typedef struct {
	int address;
	int bin_machine_code;
	char extern_name[MAX_LABEL_LEN];
} code_entry;

/*a struct representing the code table*/
typedef struct {
	code_entry *code_entries;
	int ic;
	int extern_flag;
} code_table;

code_table *code_table_init();
void code_table_free(code_table*);
error_value add_code(code_table*, const int);
int get_reg_val(const char*);
int get_op_allowed_dest(const char*);
int get_op_allowed_src(const char*);
int get_op_value(const char*);
int get_op_num_of_params(const char*);

#endif
