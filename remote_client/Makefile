# Makefile for backdoor_client
OUTPUT = client
CC=gcc -g

$(OUTPUT): $(OUTPUT).o
	$(CC) -o $(OUTPUT) $(OUTPUT).o
	rm -f *.o

all: clean $(OUTPUT) run

$(OUTPUT).o:
	$(CC) -c $(OUTPUT).c

clean:
	rm -f *.o core $(OUTPUT)

run:
	./$(OUTPUT)