#
# Makefile ESQUELETO
#
# DEVE ter uma regra "all" para geração da biblioteca
# regra "clean" para remover todos os objetos gerados.
#
# NECESSARIO adaptar este esqueleto de makefile para suas necessidades.
#
#

CC=gcc
LIB_DIR=./lib
INC_DIR=./include
BIN_DIR=./bin
SRC_DIR=./src

all: clean compile_to_object_file generate_static_library

clean:
	@clear
	@echo cleaning bin/ and lib/
	@cp bin/apidisk.o lib/
	@cp bin/bitmap2.o lib/
	@cp bin/support.o lib/
	@rm -rf bin/*
	@cp lib/apidisk.o bin/
	@cp lib/bitmap2.o bin/
	@cp lib/support.o bin/
	@rm -rf lib/*

compile_to_object_file:
	@echo compiling object files
	@gcc -c src/mft_manager.c -I include -o bin/mft_manager.o
	@gcc -c src/implementation.c -I include -o bin/implementation.o
	@gcc -c src/t2fs.c -I include -o bin/t2fs.o
	
generate_static_library:
	@echo generating static library
	@ar crs lib/libt2fs.a bin/t2fs.o bin/mft_manager.o bin/apidisk.o bin/bitmap2.o bin/support.o bin/implementation.o
	
main:
	@gcc teste/main.c -I include -L lib -lt2fs -o teste/main
	@./teste/main

test-default:
	@gcc teste/default.c -I include -L lib -lt2fs -o teste/default
	@./teste/default
	
test:
	@gcc teste/test.c -I include -L lib -lt2fs -o teste/test
	@./teste/test

mft:
	@gcc teste/testMFT.c -I include -L lib -lt2fs -o teste/testMFT
	@./teste/testMFT
