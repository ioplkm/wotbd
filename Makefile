CC=clang
all: main.c
	$(CC) main.c -lcurl -ljson-c -Wall -std=c99 -o wotb -O3
install:
	cp wotb /usr/local/bin
	chmod +x /usr/local/bin/wotb
