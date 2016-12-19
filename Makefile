all: ush

ush: parse.o main.o
	gcc -w -o ush parse.o main.o
	
parse.o: parse.h parse.c
	gcc -w -c -o parse.o parse.c
	
main.o: parse.h main.c
	gcc -w -c -o main.o main.c
	
clean:
	rm -rf parse.o main.o ush