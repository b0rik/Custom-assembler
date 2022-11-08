#ifndef UTILS_H
#define UTILS_H

#include "defs.h"
#include "error.h"
#include "symtable.h"

error_value line_valid(const char*);
int is_empty_line(const char*);
int is_comment_line(const char*);
error_value valid_label(const char*, symtable*);
error_value valid_instruction(const char*);
int is_reserved_word(const char*);
error_value valid_macro_name(const char*, symtable*);
int is_legal_number(const char*);
int is_valid_string(const char*);
int is_valid_data_params(const char*, symtable*);
int is_adjacent_commas(const char*);
int is_valid_instruction_param(const char*, symtable*);
int is_array_param(const char*, symtable*);
void get_arr_name(const char*, char*);
void get_arr_index(const char*, char*);
char *find_eq_sign(char*);

#endif
