CC = clang
CFLAGS = -std=c99 -O3 -Wall -Wextra -Wpedantic -s
LDLIBS = -lcurl -ljson-c
PREFIX = /usr/local
all: daemon client
daemon: daemon.c struct.h
	$(CC) $(CFLAGS) $(LDLIBS) daemon.c -o wotbd
client: client.c struct.h
	$(CC) $(CFLAGS) $(LDLIBS) client.c -o wotb
clean:
	rm wotbd wotb
install:
	install -m755 wotbd $(PREFIX)/bin
uninstall:
	rm $(PREFIX)/bin/wotbd
