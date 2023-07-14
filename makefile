.PHONY: game

# To compile for HTML5
# Prereqs:
# - Have emscripten install in ~/Desktop/emsdk
# - Have raylib source in ~/Desktop/raylib
# - Compile raylib with emscripten and have output file in ~/Desktop/raylib/src.
# Steps:
# 1. Uncomment the line `// #define PLATFORM_WEB`.
# 2. Open terminal.
# 3. Go to ~Desktop/emsdk.
# 4. `./emsdk activate latest`
# 5. `source "/Users/addisonmink/Desktop/emsdk/emsdk_env.sh"`
# 6. Go to the directory containing this file.
# 7. `make web`

game:
	$(CC) main.c \
	$(shell pkg-config --libs --cflags raylib) -o Game
	./Game

web:
	emcc main.c \
	-I ~/Desktop/raylib/src/ \
	~/Desktop/raylib/src/libraylib.a \
	-s USE_GLFW=3 -s WASM=1 -s USE_WEBGL2=1 -o Game.html
