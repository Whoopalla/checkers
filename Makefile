CC=gcc
CFLAGS= -std=c17 -Wall -g -ggdb 
LRaylib= -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

build: 
	$(CC) $(CFLAGS) ./src/utils/byteprint.c -I ./src/utils/ \
	./src/checkers.c ./src/checkers_ui.c -o ./build/checkers $(LRaylib)
