CC = clang
CFLAGS = -std=c99 -O3 -Wall -Wextra -Wpedantic -s -D_POSIX_C_SOURCE=2
LDLIBS = -lcurl -ljson-c
PREFIX = /usr/local
all: daemon client
daemon: daemon.c struct.h
	mkdir -p build
	$(CC) $(CFLAGS) $(LDLIBS) daemon.c -o build/wotbd
client: client.c struct.h config.h crutch.h
	mkdir -p build
	$(CC) $(CFLAGS) $(LDLIBS) client.c -o build/wotb
clean:
	rm -r build
install:
	install -m755 build/wotbd /usr/libexec/
	install -m755 build/wotb $(PREFIX)/bin
	install -m755 wotbd /etc/init.d
uninstall:
	rm /usr/libexec/wotbd
	rm $(PREFIX)/bin/wotb
	rm /etc/init.d/wotbd
