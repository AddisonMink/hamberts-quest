.PHONY: game

game:
	$(CC) main.c \
	panels.c \
	$(shell pkg-config --libs --cflags raylib) -o Game
	./Game