CFLAGS = -Wall -std=c99 -g -Wno-incompatible-pointer-types
LDFLAGS = -pthread
WRAP = wrap/os.c

data-files: data-files.c
	gcc $(CFLAGS) $(LDFLAGS) $< $(WRAP) -o $@

clean:
	rm data-files
