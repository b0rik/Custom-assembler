assembler : assembler.o code.o data.o encoder.o error.o file_handler.o memory_image.o parser.o pass1.o pass2.o symtable.o utils.o
	gcc -g -ansi -pedantic -Wall assembler.o code.o data.o encoder.o error.o file_handler.o memory_image.o parser.o pass1.o pass2.o symtable.o utils.o -o assembler

assembler.o : assembler.c defs.h file_handler.h error.h symtable.h memory_image.h pass1.h pass2.h
	gcc -c -ansi -pedantic -Wall assembler.c -o assembler.o

code.o : code.c code.h error.h
	gcc -c -ansi -pedantic -Wall code.c -o code.o

data.o : data.c data.h symtable.h error.h
	gcc -c -ansi -pedantic -Wall data.c -o data.o

encoder.o : encoder.c encoder.h utils.h
	gcc -c -ansi -pedantic -Wall encoder.c -o encoder.o

error.o : error.c error.h
	gcc -c -ansi -pedantic -Wall error.c -o error.o

file_handler.o : file_handler.c file_handler.h error.h encoder.h
	gcc -c -ansi -pedantic -Wall file_handler.c -o file_handler.o

memory_image.o : memory_image.c memory_image.h
	gcc -c -ansi -pedantic -Wall memory_image.c -o memory_image.o

parser.o : parser.c parser.h utils.h memory_image.h
	gcc -c -ansi -pedantic -Wall parser.c -o parser.o

pass1.o : pass1.c pass1.h parser.h encoder.h utils.h
	gcc -c -ansi -pedantic -Wall pass1.c -o pass1.o

pass2.o : pass2.c pass2.h utils.h encoder.h
	gcc -c -ansi -pedantic -Wall pass2.c -o pass2.o

symtable.o : symtable.c symtable.h 
	gcc -c -ansi -pedantic -Wall symtable.c -o symtable.o

utils.o : utils.c utils.h memory_image.h
	gcc -c -ansi -pedantic -Wall utils.c -o utils.o

