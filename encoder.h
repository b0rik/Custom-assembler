#ifndef ENCODER_H
#define ENCODER_H

#include "defs.h"
#include "error.h"
#include "symtable.h"
#include "memory_image.h"

/*enum of the locations of every part of the encoded word*/
typedef enum{
	CODING_MODE = 0,
	DEST_ADDRESSING_MODE = 2,
	SRC_ADDRESSING_MODE = 4,
	OP_CODE = 6,
	UNUSED = 10,
	SRC_REG = 5,
	DEST_REG = 2
}word_loc;

/*macros for encoding words*/
#define encode_op(op) ((int)(op << OP_CODE))
#define encode_src_mode(src_mode) ((int)(src_mode << SRC_ADDRESSING_MODE))
#define encode_dest_mode(dest_mode) ((int)(dest_mode << DEST_ADDRESSING_MODE))
#define encode_src_reg(reg) ((int)(reg << SRC_REG))
#define encode_dest_reg(reg) ((int)(reg << DEST_REG))

error_value encode_instruction(char*, symtable*, memory_image*);
void word_to_4_special_base(int, char*);
int get_addr_mode(char*, symtable*);
int get_addr_mode_val(int);

#endif
