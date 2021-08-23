all: main clear

main: casamento.o main.o
	gcc -g casamento.o main.o -o tp4 -pthread

gen: casamento.o gen.o
	gcc casamento.o gen.o -o gen -pthread

gen.o: gen.c casamento.h
	gcc -g -c gen.c

main.o: main.c casamento.h
	gcc -g -c main.c

casamento.o: casamento.c casamento.h
	gcc -g -c casamento.c -pthread

clear:
	rm *.o
