#include "defs.h"
#include "file_handler.h"
#include "error.h"
#include "symtable.h"
#include "memory_image.h"
#include "pass1.h"
#include "pass2.h"

/* entry point */
int main(int argc, char **argv) {
	int 		 i;
	FILE 		 *fp;
	error_value  err_val = NO_ERROR;
	memory_image *memory_image_p;
	symtable     *symtable_p;
	char 	     file_name[MAX_FILE_NAME_LEN];

	/*check if files were provided to procces*/
	if (argc <= 1) {
		print_error(NO_PARAMETERS, 0, 0);
		return EXIT_FAILURE;
	}

	/*itterate over the provided files and procces them*/
	for (i = 1; i < argc; i++) {
		/*create a full file name with .as extention from provided base name*/
		make_file_name(argv[i], CODE_FILE_EXT, file_name);

		/*check if the file exists*/
		if (!(err_val = file_exists(file_name))) {

			/*if the file exists try to open it to read*/
			if ((fp = fopen(file_name, READ))) {

				/*if file was successfully opened try to initialize the memory image
				 * and the symtable*/
				if ((memory_image_p = memory_image_init()) &&
						(symtable_p = symtable_init())) {

					/*if successfully initialized then execute first pass on the given file*/
					if (!(err_val = pass1_execute(fp, memory_image_p,
							symtable_p, file_name))) {

						/*if no errors occured during the first pass then prepare for
						 * the second pass and execute it on the given file*/
						pass2_prep(fp, memory_image_p, symtable_p);
						if (!(err_val = pass2_execute(fp, memory_image_p,
								symtable_p, file_name)))

							/*if no error occured during the second pass the create
							 * the object file and if needed then the externals and
							 * entries files*/
							err_val = create_files(argv[i], memory_image_p,
									symtable_p);
					}
					/*free the initialized memory_image and symtable*/
					memory_image_free(memory_image_p);
					symtable_free(symtable_p);
					fclose(fp);
				} else {

					/*if couldnt initialize the memory image of the symtable
					 * close the file and throw an error*/
					fclose(fp);
					err_val = ERROR_MEMORY_ALLOC;
				}
			} else
				/*if couldn open the file throw an error*/
				err_val = ERROR_OPEN_FILE;
		}

		print_error(err_val, file_name, 0);
		printf("\n");
	}

	return EXIT_SUCCESS;
}
