#ifndef MEMORY_IMAGE_H
#define MEMORY_IMAGE_H

#include "defs.h"
#include "data.h"
#include "code.h"

/*a struct representing a memory image*/
typedef struct{
	code_table *code;
	data_table *data;
}memory_image;

memory_image *memory_image_init();
void memory_image_free(memory_image*);

#endif
