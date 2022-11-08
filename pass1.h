#ifndef PASS1_H
#define PASS1_H

#include "error.h"
#include "memory_image.h"
#include "symtable.h"

error_value pass1_execute(FILE*, memory_image*, symtable*, const char*);

#endif
