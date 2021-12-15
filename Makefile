CC = gcc
server: server.o handle_file.o x264.o
	$(CC) -o server server.o handle_file.omake 
client: client.o handle_file.o x264.o
	$(CC) -o client client.o handle_file.o
server.o: server.c handle_file.h
	$(CC) -c server.c
server.o: client.c handle_file.h
	$(CC) -c client.c
handle_file.o: handle_file.c handle_file.h
	$(CC) -c handle_file.c
x264.o: x264.c x264.h
	$(CC) -c x264.c


clean:	server client
	rm server
	rm client

all: server client