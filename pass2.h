#ifndef PASS2_H
#define PASS2_H

#include "error.h"
#include "memory_image.h"
#include "symtable.h"

error_value pass2_execute(FILE*, memory_image*, symtable*, const char*);
void pass2_prep(FILE*, memory_image*, symtable*);

#endif
