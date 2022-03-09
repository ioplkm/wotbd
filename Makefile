CC=clang
all: main.c
	$(CC) main.c -lcurl -ljson-c -Wall -std=c99
