# Makefile for hello.c
GCC=/usr/bin/gcc

mem_get: mem_get.c
	$(GCC) -Wall -O2 -o mem_get mem_get.c
	$(GCC) -Wall -O2 -pthread cpu_get.c -o cpu_get

clean:
	rm -f mem_get cpu_get
