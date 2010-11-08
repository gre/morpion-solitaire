DEBUG = -Wall -g
OPT = ${DEBUG}

all: clean morpion

%.o : %.c
	gcc -c $< -o $@ $(OPT)

morpion: main.c game.o ui.o export.o utils.o globals.h
	gcc main.c game.o ui.o export.o utils.o -lcurses -o morpion $(OPT)

clean:
	rm -f *.o
