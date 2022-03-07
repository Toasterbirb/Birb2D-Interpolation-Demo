CC=g++
outputDir=./build
CFLAGS=-O3
WarningFlags=-Wpedantic -pedantic -Wall -Wextra
SDL_FLAGS=-lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx

all: path_gen

path_gen: main.o
	mkdir -p build
	rsync -av ./res ./build/
	$(CC) $^ -lbirb2d $(CFLAGS) $(SDL_FLAGS) $(WarningFlags) -o $(outputDir)/path_gen

main.o: ./src/main.cpp
	$(CC) -c $^ $(CFLAGS) -o main.o

clean:
	rm -rf build
	rm -f *.o
