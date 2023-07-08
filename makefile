.PHONY: game

game:
	$(CC) main.c \
	 	$(shell pkg-config --libs --cflags raylib) -o Game
	./Game