# TODO : refactor dependencies

.PHONY: clean

DEBUG = -Wall -g
OPT = ${DEBUG}

all: clean morpion

points.o: points.c points.h
	gcc -c points.c -o $@ $(OPT)

highscore.o: highscore.c highscore.h
	gcc -c highscore.c -o $@ $(OPT)

game.o : game.c game.h globals.h points.o highscore.o
	gcc -c game.c -o $@ $(OPT)

export.o : export.c export.h game.h globals.h
	gcc -c export.c -o $@ $(OPT)

ui.o : ui.c ui.h game.h globals.h points.o
	gcc -c ui.c -o $@ $(OPT)

utils.o : utils.c utils.h
	gcc -c utils.c -o $@ $(OPT)

morpion: main.c game.o ui.o export.o utils.o points.o highscore.o globals.h
	gcc main.c game.o ui.o export.o utils.o points.o highscore.o -lcurses -lm -o $@ $(OPT)

clean:
	rm -f *.o
