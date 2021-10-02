
all: prog

prog: build/fileman.o build/main.o bin
	gcc build/main.o build/fileman.o -o bin/prog 

build/fileman.o: src/fileman.c src/fileman.h build
	gcc -Wall -Werror -c src/fileman.c -o build/fileman.o

build/main.o: src/main.c src/fileman.h build
	gcc -Wall -Werror -c src/main.c -o build/main.o

bin: 
	mkdir bin

build:
	mkdir build

clean:
	rm -rf build bin

.PHONY: all clean
