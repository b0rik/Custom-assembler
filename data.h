#ifndef DATA_H
#define DATA_H

#include "defs.h"
#include "symtable.h"
#include "data.h"

/*a struct representing an entry of the data table*/
typedef struct{
	int address;
	int value;
} data_entry;

/*a struct representing the data table*/
typedef struct{
	data_entry *data_entries;
	int dc;
}data_table;

data_table *data_table_init();
void data_table_free(data_table*);
error_value add_string_data(data_table*, const char*);
error_value add_num_data(data_table*, char*, symtable*);
void update_data_addr(data_table*, const int);
error_value valid_directive(const char*);

#endif
