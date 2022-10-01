CC = clang
CFLAGS = -std=c99 -O3 -Wall -Wextra -Wpedantic -s
LDLIBS = -lcurl -ljson-c
PREFIX = /usr/local
all: main.c
	$(CC) $(CFLAGS) $(LDLIBS) main.c -o wotbd
clean:
	rm wotbd
install:
	install -m755 wotbd $(PREFIX)/bin
uninstall:
	rm $(PREFIX)/bin/wotbd
