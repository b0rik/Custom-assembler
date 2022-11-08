#include "memory_image.h"

/* memory_image_init : allocate a memory image and initialize it
 * parameters        :
 * return            : if initialized succesfully return a pointer to the memory image
 * 					   else return NULL
 */
memory_image *memory_image_init() {
	memory_image *mem_img;

	/* try to initialize a code table and data table and if succefulll then return a pointer
	 * to the memory image else return NULL*/
	return (mem_img = malloc(sizeof(memory_image))) &&
		   (mem_img->code = code_table_init()) &&
		   (mem_img->data = data_table_init()) ? mem_img : NULL;
}

/* memory_image_free : free a prevoiusly allocated memory_image
 * parameters        : mem_img - a pointer to a memory image to free*/
void memory_image_free(memory_image *mem_img) {
	if(mem_img){
		data_table_free(mem_img->data);
		code_table_free(mem_img->code);
		free(mem_img);
	}
}
