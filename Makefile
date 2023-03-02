general_compile_options = -Werror -Wextra -Wall 
style_compile_options = -Wparentheses -pedantic -Wunused-variable
variable_compile_options = -Wfloat-equal -Wconversion -Wunused-parameter -Wredundant-decls -Wunused-value -Wuninitialized  
func_compile_options = -Wreturn-type -Wunused-function -Wswitch-default  -Winit-self
  
CC = gcc
#CFLAGS = -O3 -g -Wall -Werror -Wextra -Wall -Wfloat-equal -Wconversion -Wparentheses -pedantic -Wunused-parameter -Wunused-variable -Wreturn-type -Wunused-function -Wredundant-decls -Wreturn-type -Wredundant-decls -Wunused-value -Wswitch-default -Wuninitialized -Winit-self
CFLAGS = -O3 -g $(general_compile_options) $(style_compile_options) $(variable_compile_options) $(func_compile_options)
DEPS = eeprom.h
OBJ = eeprom.o

all: main.o $(OBJ)
	$(CC) main.o $(OBJ) -o main

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

run: main.o eeprom.o
	rm -rf *o
	chmod a+x main
	./main

clean:
	rm -rf *o main
	rm -rf eeprom.txt