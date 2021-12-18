CC = gcc
server: server.o handle_file.o x264.o
	$(CC) -o server server.o handle_file.o x264.o linked_list.c
client: client.o handle_file.o x264.o
	$(CC) -o client client.o handle_file.o x264.o
server.o: server.c handle_file.h
	$(CC) -c server.c
client.o: client.c handle_file.h
	$(CC) -c client.c
handle_file.o: handle_file.c handle_file.h
	$(CC) -c handle_file.c
x264.o: x264.c x264.h
	$(CC) -c x264.c
linked_list.o: linked_list.c linked_list.h
	$(CC) -c linked_list.c
clean:	server client
	rm server
	rm client

all: server client