#ifndef HANDLE_FILE_H_
#define HANDLE_FILE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "linked_list.h"
extern const int msg_len;
int search_file(char *dir_name, char* file_name);
//int send_file(int client_socket, FILE* fp);
int send_vid(int client_socket, vid_obj_t* vid_obj);
int recv_file(int socket, char* path_264);
void recv_msg(int socket, char* buf, int buf_len);
void send_msg(int socket, char* buf, int buf_len);
long get_file_size(char* file_path);
#endif