main: main.cc
	g++ -O3 -Wall -Wextra main.cc -o main

run: main
	./main
