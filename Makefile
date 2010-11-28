.PHONY: clean

DEBUG = -Wall -g
OPT = ${DEBUG}

all: clean morpion

game.o : game.c game.h globals.h
	gcc -c game.c -o $@ $(OPT)

export.o : export.c export.h game.h globals.h
	gcc -c export.c -o $@ $(OPT)

ui.o : ui.c ui.h game.h globals.h
	gcc -c ui.c -o $@ $(OPT)

utils.o : utils.c utils.h
	gcc -c utils.c -o $@ $(OPT)

morpion: main.c game.o ui.o export.o utils.o globals.h
	gcc main.c game.o ui.o export.o utils.o -lcurses -lm -o $@ $(OPT)

clean:
	rm -f *.o
