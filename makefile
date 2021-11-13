all: compile execute

compile:
	gcc main.c -o main `pkg-config fuse --cflags --libs`

execute:
	./main -f mount