#include "file_handler.h"
#include "error.h"
#include "encoder.h"

/* create_extern_file : create the externals file
 * parameters         : file_base    - the base file name
 * 						code_table_p - a pointer to a code table
 * return             : NO_ERROR          - if created the file succesfully
 * 						ERROR_CREATE_FILE - if an error occured creating the file*/
static error_value create_extern_file(const char *file_base, code_table *code_table_p) {
	error_value     err_val = NO_ERROR;
	char 		    file_name[MAX_FILE_NAME_LEN];
	int			    i;
	FILE	        *fp;

	/*create the file name with .ext extension*/
	make_file_name(file_base, EXTERNALS_FILE_EXT, file_name);

	/*try to create the file*/
	if ((fp = fopen(file_name, WRITE_APPEND))) {

		/*if succesfully created the file itterate over the code table
		 * and add every value flagged as external to the file*/
		for (i = 0; i < code_table_p->ic; i++)
			if (code_table_p->code_entries[i].bin_machine_code == 1)
				fprintf(fp, "%s\t%04d\n",
						code_table_p->code_entries[i].extern_name,
						code_table_p->code_entries[i].address);

		fclose(fp);
	} else /*if unable to create the file*/
		err_val = ERROR_CREATE_FILE;

	return err_val;

}

/* create_entry_file : create the entries file
 * parameters        : file_base  - the base file name
 * 					   symtable_p - a pointer to a symbol table
 * return            : NO_ERROR          - if created the file succesfully
 * 				       ERROR_CREATE_FILE - if an error occured creating the file*/
static error_value create_entry_file(const char *file_base, symtable *symtable_p) {
	error_value err_val = NO_ERROR;
	char	    file_name[MAX_FILE_NAME_LEN];
	int 		i;
	FILE	    *fp;

	/*create the file name with .ent extension*/
	make_file_name(file_base, ENTRIES_FILE_EXT, file_name);

	/*try to create the file*/
	if ((fp = fopen(file_name, WRITE_APPEND))) {

		/* if succesfully created the file itterate over the symbol table
		 * and add every value flagged as entry to the file*/
		for (i = 0; i < symtable_p->table_size; i++)
			if (symtable_p->symtable_entries[i].type == ENTRY)
				fprintf(fp, "%s\t%04d\n", symtable_p->symtable_entries[i].name,
										 symtable_p->symtable_entries[i].value);
		fclose(fp);
	} else /*if unable to create the file*/
		err_val = ERROR_CREATE_FILE;

	return err_val;
}

/* create_object_file : create the object file
 * parameters         : file_base  - the base file name
 * 					    mem_img    - a pointer to a memory image
 * return             : NO_ERROR          - if created the file succesfully
 * 				        ERROR_CREATE_FILE - if an error occured creating the file*/
static error_value create_object_file(const char *file_base, memory_image *mem_img) {
	error_value err_val = NO_ERROR;
	char 	    base_4_word[WORD_SIZE / 2 + 1];
	char 		file_name[MAX_FILE_NAME_LEN];
	FILE 		*fp;
	int 		i;

	/*create the file name with .ob extension*/
	make_file_name(file_base, OBJECT_FILE_EXT, file_name);

	/*try to create the file*/
	if ((fp = fopen(file_name, WRITE_APPEND))) {
		/* if succesfully created the file itterate over the code table
		 * and add every value to the file*/
		fprintf(fp, "%4d %d\n", mem_img->code->ic, mem_img->data->dc);
		for (i = 0; i < mem_img->code->ic; i++) {
			word_to_4_special_base(
					mem_img->code->code_entries[i].bin_machine_code,
					base_4_word);
			fprintf(fp, "%04d %s\n", mem_img->code->code_entries[i].address,
									 base_4_word);
		}

		/*itterate over the data table and add every value to the file*/
		for (i = 0; i < mem_img->data->dc; i++) {
			word_to_4_special_base(mem_img->data->data_entries[i].value,
								   base_4_word);
			fprintf(fp, "%04d %s\n", mem_img->data->data_entries[i].address,
								     base_4_word);
		}

		fclose(fp);
	} else /*if unable to create the file*/
		err_val = ERROR_CREATE_FILE;

	return err_val;
}

/* make_file_name : make a full file name from the base and a given extension
 * parameters     : base 		  - the base of the file name
 * 					ext  	      - the extension of the file
 * 					file_name_out - the output for the full file name
 * return         :
 */
void make_file_name(const char *base, const char *ext, char *file_name_out) {
	strcpy(file_name_out, base);
	strcat(file_name_out, ext);
}

/* file_exists : check if a given file exists
 * parameters  : file_name - the name of the file to check
 * return      : NO_ERROR          - if the file exists
 * 				 INVALID_FILE_NAME - if the file doesnt exists*/
error_value file_exists(const char *file_name) {
	FILE 	    *fp;
	/*try to open the file to read*/
	error_value err_val = ((fp = fopen(file_name, READ))) ? NO_ERROR : INVALID_FILE_NAME;

	/*if file opened then close it*/
	if(fp)
		fclose(fp);

	return err_val;
}
/* create_files : create the object file and if needed then the entries and externals files
 * parameters   : file_base  - the base of the files names
 * 				  mem_img    - a pointer to a memory image
 * 				  symtable_p - a pointer to a symbol table
 * return       : NO_ERROR          - if the files created succesfully
 * 				  ERROR_CREATE_FILE - if there was an error creating the files*/
error_value create_files(const char *file_base, memory_image *mem_img,
						 symtable *symtable_p) {
	error_value err_val = NO_ERROR;

	/*try to create the object file*/
	if (!(err_val = create_object_file(file_base, mem_img))) {
		/*if created then check if externals file is needed*/
		if (mem_img->code->extern_flag)
			/*if needed try to create it*/
			err_val = create_extern_file(file_base, mem_img->code);
		/*check if entries file is needed*/
		if (!err_val && symtable_p->entry_flag)
			/*if needed try to create it*/
			err_val = create_entry_file(file_base, symtable_p);
	}

	return err_val;
}
