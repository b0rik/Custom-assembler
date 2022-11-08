#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "error.h"
#include "memory_image.h"
#include "symtable.h"

/*tokens for fopen*/
#define READ "r"
#define WRITE_APPEND "w+"

/*file extensions*/
#define CODE_FILE_EXT ".as"
#define OBJECT_FILE_EXT ".ob"
#define EXTERNALS_FILE_EXT ".ext"
#define ENTRIES_FILE_EXT ".ent"

void make_file_name(const char*, const char*, char*);
error_value file_exists(const char*);
error_value create_files(const char*, memory_image*, symtable*);

#endif
