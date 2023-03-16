CFLAGS = -Wall -std=c99 -g -Wno-incompatible-pointer-types
LDFLAGS = -pthread
WRAP = wrap/os.c

rwlock: rwlock.c
	gcc $(CFLAGS) $(LDFLAGS) $< $(WRAP) -o $@

clean:
	rm rwlock

