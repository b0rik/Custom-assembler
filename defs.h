#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>

/*boolean expresions*/
#define TRUE 1
#define FALSE 0

/*max length of a line*/
#define MAX_LINE_LEN 80

/*the size of a memory word*/
#define WORD_SIZE 14

/*max length of a label*/
#define MAX_LABEL_LEN 31

/*max length of a file name*/
#define MAX_FILE_NAME_LEN 4096

/*max size of a number*/
#define MAX_NUM_SIZE 4

/*the address offset we assumbe the program starts from*/
#define ADDRESS_OFFSET 100

/*parsing tokens for strtok*/
#define PARSING_WHITESPACE_TOKENS " \t\n\r"
#define PARSING_MACRO_TOKENS " ="
#define PARSING_END_OF_LINE_TOKENS "\n\r"
#define PARSING_DATA_NUM_PARAMS_TOKENS " ,\n\t\r"
#define PARSING_PARAMS_TOKENS " ,\n\t\r"

/*addressing mode for bitwise operators use*/
typedef enum{
	NONE = 0,
	IMMEDIATE = 1,
	DIRECT = 2,
	INDEX = 4,
	REGISTER = 8
} addressing_mode;

#endif

