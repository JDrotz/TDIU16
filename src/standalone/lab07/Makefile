CFLAGS = -Wall -std=c99 -g -Wno-incompatible-pointer-types
LDFLAGS = -pthread
WRAP = wrap/os.c

exec-wait: exec-wait.c
	gcc $(CFLAGS) $(LDFLAGS) $< $(WRAP) -o $@

clean:
	rm exec-wait

