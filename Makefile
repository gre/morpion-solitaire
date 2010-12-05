.PHONY: clean

DEBUG = -Wall -g
OPT = ${DEBUG}

all: clean morpion

utils.o : utils.c utils.h
	gcc -c utils.c -o $@ $(OPT)

points.o: points.c points.h utils.h globals.h
	gcc -c points.c -o $@ $(OPT)

highscore.o: highscore.c highscore.h globals.h
	gcc -c highscore.c -o $@ $(OPT)

export.o : export.c export.h game.h globals.h
	gcc -c export.c -o $@ $(OPT)

ui.o : ui.c ui.h globals.h game.h points.h
	gcc -c ui.c -o $@ $(OPT)

game.o : game.c game.h globals.h utils.h export.h points.h highscore.h
	gcc -c game.c -o $@ $(OPT)
	
morpion: main.c game.o ui.o export.o utils.o points.o highscore.o globals.h
	gcc main.c game.o ui.o export.o utils.o points.o highscore.o -lcurses -lm -o $@ $(OPT)

clean:
	rm -f *.o
