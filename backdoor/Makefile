# Makefile for pkt_sniffer
OUTPUT = backdoor
CC=gcc -g
CLIB=-lpcap

$(OUTPUT): $(OUTPUT).o packet_handler.o tcp_sender.o
	$(CC) -o $(OUTPUT) $(OUTPUT).o packet_handler.o tcp_sender.o $(CLIB)
	rm -f *.o

all: clean $(OUTPUT) run

clean:
	rm -f *.o core $(OUTPUT)

$(OUTPUT).o:
	$(CC) -c $(OUTPUT).c

run:
	./$(OUTPUT) "udp dst port 383"

packet_handler.o: 
	$(CC) -c packet_handler.c

tcp_sender.o:
	$(CC) -c tcp_sender.c
