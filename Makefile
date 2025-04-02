
all: 
	cc -std=c99 -O3 -Wall -Wextra -Wpedantic $(wildcard src/*.c)

