default: build

build: main.c nonblock.c nonblock.h
	gcc -Wall main.c nonblock.c -o snk

run: build
	./snk

debug: main.c nonblock.c nonblock.h
	gcc -Wall -DDEBUG -Q -g -O0 -o snk main.c nonblock.c
	./snk

clean:
	rm snk *~ main.c.* nonblock.c.*
