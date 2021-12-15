#ifndef HANDLE_FILE_H
#define HANDLE_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SIZE 65535

int search_file(char *dir_name, char* file_name);
int send_file(int client_socket, FILE* fp);
int recv_file(int socket, char* path_264);
#endif