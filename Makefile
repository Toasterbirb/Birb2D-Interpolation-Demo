CC=g++
outputDir=./build
WarningFlags=-Wpedantic -pedantic -Wall -Wextra
SDL_FLAGS=-lSDL2 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2_gfx

all: path_gen

path_gen: main.o
	mkdir -p build
	rsync -av ./res ./build/
	$(CC) $^ $(SDL_FLAGS) -lbirb2d $(WarningFlags) -o $(outputDir)/path_gen

main.o: ./src/main.cpp
	$(CC) -c $^ -o main.o

clean:
	rm -rf build
	rm -f *.o
