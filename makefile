compile:
	gcc main.c -o main `pkg-config fuse --cflags --libs`