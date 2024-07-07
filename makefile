CFLAGS= -Wall -Wextra -pedantic -Werror


all:
	gcc chip8.c -o chip8 $(CLFLAGS) `sdl2-config --cflags --libs`


clean:
	rm -rf chip8 && clear

run:	
	./chip8
