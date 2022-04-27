CC = clang
CFLAGS = -std=c99 -O3 -Wall -Wextra -Wpedantic
LDFLAGS = -lcurl -ljson-c
all: main.c
	$(CC) $(CFLAGS) $(LDFLAGS) main.c -o wotb
install:
	cp wotb /usr/local/bin
	chmod +x /usr/local/bin/wotb
clean:
	rm wotb
