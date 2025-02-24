CC = clang
CFLAGS = -g -Wall -std=c11 -lm 

FLEX_DIR = src/parser
FLEX_SRC = $(FLEX_DIR)/SysY.l
FLEX_GEN = $(FLEX_DIR)/lex.yy.c

BISON_DIR = src/parser
BISON_SRC = $(BISON_DIR)/SysY.y
BISON_GEN_H = $(BISON_DIR)/SysY.tab.h
BISON_GEN_C = $(BISON_DIR)/SysY.tab.c

SYSY_DEF_DIR = src/SysY.type
SYSY_DEF_SRC = 	$(SYSY_DEF_DIR)/*.c

SYSY_SYMTAB_DIR = src/SysY.symtab
SYSY_SYMTAB_SRC = $(SYSY_SYMTAB_DIR)/*.c

SYSY_TARGET_DIR = src/SysY.target
SYSY_TARGET_SRC = $(SYSY_TARGET_DIR)/*.c

OPTIMIZER_DIR = src/optimizer
OPTIMIZER_SRC = $(OPTIMIZER_DIR)/*.c

UTILS_DIR = src/utils
UTILS_SRC = $(UTILS_DIR)/*.c

REG_ALLOCATOR_DIR = src/RegAllocator
REG_ALLOCATOR_SRC = $(REG_ALLOCATOR_DIR)/*.c

IR = src/ssa/*.c

MAIN = src/main.c src/init.c

compiler: $(BISON_GEN_H) $(BISON_GEN_C) $(FLEX_GEN) $(SYSY_DEF_SRC) $(UTILS_SRC) $(SYSY_SYMTAB_SRC) $(IR) $(SYSY_TARGET_SRC) $(OPTIMIZER_SRC) $(REG_ALLOCATOR_SRC) $(MAIN)
	$(CC) $(CFLAGS) $^ 
	mv ./a.out $@
	objdump -d -S $@ > tmp_parser.asm

test: compiler
	./compiler -S -o  tmp.s tmp.c
	gcc -g -march=armv7-a ./tmp.s -o tmp.o -L -lsysy ./libsysy.a -static
	gdb --args ./tmp.o

$(BISON_GEN_H) $(BISON_GEN_C): $(BISON_SRC)
	bison -d $< -o $(BISON_GEN_H) -o $(BISON_GEN_C)

$(FLEX_GEN): $(FLEX_SRC)
	flex -o $@ $<

clean:
	rm -f $(BISON_GEN_H) $(BISON_GEN_C) $(FLEX_GEN) compiler

debug: compiler
	gdb --args ./compiler -S -o tmp.s tmp.c

leak: compiler
	valgrind 2>tmp.out --leak-check=full --show-leak-kinds=all --tool=memcheck -v --track-origins=yes  ./compiler -S -o  tmp.s tmp.c 
	code ./tmp.out

valgrind: compiler
	valgrind 2>tmp.out --tool=memcheck -v --track-origins=yes  ./compiler -S -o  tmp.s tmp.c 
	code ./tmp.out