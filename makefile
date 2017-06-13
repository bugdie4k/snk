default: build

build: main.c nonblock.c nonblock.h
	gcc -o snk main.c nonblock.c

run: build
	./snk

clean:
	rm snk *~
