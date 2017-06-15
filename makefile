default: build

build: main.c nonblock.c nonblock.h
	gcc -o snk main.c nonblock.c

run: build
	./snk

debug: main.c nonblock.c nonblock.h
	gcc -DDEBUG -Q -g -O0 -o snk main.c nonblock.c
	./snk

clean:
	rm snk *~ main.c.* nonblock.c.*
