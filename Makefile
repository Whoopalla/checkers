CC=gcc
CFLAGS= -std=c17 -Wall -g -ggdb -lGL -lX11 -lpthread -lXrandr -lXi -Wl,-rpath,'$\ORIGIN/libs/' -I include include/glad/glad.h include/GLFW/glfw3.h \
		include/GLFW/glfw3native.h include/KHR/khrplatform.h 
Libs= -L ./libs/libglfw.so ./libs/libglfw.so.3 ./libs/libglfw.so.3.4

build: 
	$(CC) $(CFLAGS) ./src/checkers_gl.c ./include/glad.c -lm $(Libs) \
	./src/utils/byteprint.c -I ./src/utils/ ./src/checkers.c -o ./build/checkers
