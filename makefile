all: compile execute

compile:
	gcc -lcrypto main.c -o main `pkg-config fuse --cflags --libs`

execute:
	./main -c -pv keys/private.txt -pb keys/public.txt -d do_args/ -s do_args/ -f mount