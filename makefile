all: compile execute

compile:
	gcc -lcrypto main.c -o main `pkg-config fuse --cflags --libs`

execute:
	./main -c tree/do_args/ -f mount