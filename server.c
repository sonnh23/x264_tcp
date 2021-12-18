#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "handle_file.h"
#include "x264.h"
#include "linked_list.h"
const int msg_len = 65536;

//int handle_dtb(int new_socket, char* file_name);
int handle_dtb(linked_list_t* vid_list);
int main(int argc, char *argv[]){
	/*if(argc != 3){
		fprintf(stderr, "Usage: \t./server <server ip> <server port>\n");
		exit(0);
	}*/
	char* server_ip = "127.0.0.1"; //argv[1];
	int server_port = 8080; //atoi(argv[2]);

	/*
	// load database infor to linked_list
	*/

	linked_list_t* vid_list = ll_create();
	handle_dtb(vid_list);
	/*
	//create welcoming socket 
	*/
	int sockfd, ret;
	struct sockaddr_in server_addr;
	int new_socket;
	struct sockaddr_in new_addr;

	socklen_t addr_size;
	pid_t child_pid;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Server Socket is created.\n");

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	server_addr.sin_addr.s_addr = inet_addr(server_ip);

	ret = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(ret < 0){
		printf("Error in binding.\n");
		exit(1);
	}
	printf("Binding sucessfully\tIP: %s\tPORT: %d\n", server_ip, server_port);
	if(listen(sockfd, 10) == 0){
		printf("Listening....\n");
	}else{
		printf("Error in listening.\n");
	}
	while(1){
		addr_size = sizeof(new_addr);
		new_socket = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
		if(new_socket < 0){
            perror("Error in connecting.\n");
			exit(1);
		}
		printf("\n[+] Connection accepted from %s\t:%d\n", inet_ntoa(new_addr.sin_addr), ntohs(new_addr.sin_port));

		if((child_pid = fork()) == 0){
			close(sockfd);
			char buffer[msg_len];
			while(1){
                bzero(buffer,msg_len);
				int n = recv(new_socket, buffer, msg_len, 0);
				if(n<=0){
					printf("Error in receiving file name from %s: %d\n",inet_ntoa(new_addr.sin_addr), ntohs(new_addr.sin_port));
					exit(1);
				}
				else{
					if(strcmp(buffer, ":q") == 0){
						close(new_socket);
						printf("[-] Disconnected from %s: %d\n", inet_ntoa(new_addr.sin_addr), ntohs(new_addr.sin_port));
						FILE *pytalk = fopen("com/pytalk.txt", "w");
						fclose(pytalk);
						exit(0);
					}else{
						//if client requests for webcam video:
						/*
						if(strcmp(buffer, "webcam") == 0){
							char *record = "ffmpeg -f v4l2 -framerate 25 -video_size 640x480 -i /dev/video0 svr_database/mp4/webcam.mp4";
							system(record);
						}*/
						char* file_name = (char*) calloc(sizeof(buffer), sizeof(char));
						strcpy(file_name, buffer);
						fprintf(stderr,"Client %s: %d is requesting for: '%s.mp4'\n", inet_ntoa(new_addr.sin_addr), ntohs(new_addr.sin_port), buffer);
						/*
						if(!handle_dtb(new_socket, file_name)){
							char* path = (char*) calloc(sizeof(file_name) +22, sizeof(char));
							sprintf(path, "svr_database/264/%s.264", file_name);
							FILE *fp = fopen(path,"rb");
							free(path);
							free(file_name);
							fprintf(stderr, "Sending '%s.264' to %s: %d ...\n", buffer, inet_ntoa(new_addr.sin_addr), ntohs(new_addr.sin_port));
							bzero(buffer, msg_len);
							send_file(new_socket, fp);
							
							fclose(fp);
						}
						*/
						vid_obj_t* vid_obj = ll_find_vid_name(vid_list, file_name);
						if(vid_obj == NULL){
							fprintf(stderr, "File '%s,mp4' does not exist\n", file_name);
							send(new_socket,"-1",2,0);
						}
						else{
							if(!vid_obj->encoded){
								fprintf(stderr, "File '%s.mp4' is does not encoded, start encoding...\n", file_name);
								if(!encode_264(file_name)){
									fprintf(stderr, "'%s.mp4' was encoded to '%s.264'\n", file_name, file_name);
									vid_obj->encoded = true;
									vid_obj->size_264 = get_file_size(vid_obj->path_264);
								}
							}
							else{
								fprintf(stderr, "File '%s.mp4' is already encoded to '%s.264'\n", file_name, file_name);
							}
							fprintf(stderr, "Sending '%s.264' to %s: %d ...\n", buffer, inet_ntoa(new_addr.sin_addr), ntohs(new_addr.sin_port));
							FILE *fp = fopen(vid_obj->path_264,"rb");
							bzero(buffer, msg_len);
							send_vid(new_socket, vid_obj);
						}
					}
				}
			}
		}
	}
	return 0;
}
#if 0
int handle_dtb(int new_socket, char* file_name){
	/*search in mp4 folder, check if video is exist or not*/
	char* dir_mp4 = (char*) calloc(17, sizeof(char));	strcpy(dir_mp4, "svr_database/mp4");
	char* dir_264 = (char*) calloc(17, sizeof(char));	strcpy(dir_264, "svr_database/264");
	char* vid_mp4 = (char*) calloc(sizeof(file_name)+4, sizeof(char));	strcpy(vid_mp4, file_name);	strcat(vid_mp4,".mp4");
	char* vid_264 = (char*) calloc(sizeof(file_name)+4, sizeof(char));	strcpy(vid_264, file_name);	strcat(vid_264,".264");
	

	//fprintf(stderr, "%s\t\t%s\t\t%s\t\t%s\t\t%s\n", dir_mp4, dir_264,file_name, vid_mp4, vid_264);
	int return_val;
	switch(search_file(dir_mp4, vid_mp4)){
		case -1: {
			fprintf(stderr, "Can not open %s\n", dir_mp4);
			send(new_socket,"-1",2,0);
			return_val = -1;
			break;
		}
		case  0:{
			fprintf(stderr, "File '%s' does not exist\n", vid_mp4);
			send(new_socket,"-1",2,0);
			return_val = -1;
			break;
			}
		case 1:{
			fprintf(stderr, "Found '%s', checking encoding...\n", vid_mp4);
			
			//search in 264 folder, check if video is encoded or not
			switch(search_file(dir_264, vid_264)){
				case -1:{
					fprintf(stderr, "Can not open %s\n", dir_264);
					send(new_socket,"-1",2,0);
					return_val = -1;
					break;
				}
				case 0:{
					fprintf(stderr, "File '%s' is does not encoded, start encoding...\n", vid_mp4);
					if(!encode_264(file_name)){
						fprintf(stderr, "'%s.mp4' was encoded to '%s.264'\n", file_name, file_name);
						return_val = 0;
					}
					else{
						fprintf(stderr, "Error in encoding '%s.mp4\n", file_name);
					return_val = -1;
					}
					break;
				}
				case 1:{
					fprintf(stderr, "File '%s' is already encoded to '%s'\n", vid_mp4, vid_264);
					return_val = 0;
					break;
				}
			}
			break;
		}
	}
	free(dir_mp4);
	free(dir_264);
	free(vid_mp4);
	free(vid_264);
	return return_val;
}
#endif
int handle_dtb(linked_list_t* vid_list){
	char* dir_mp4 = (char*) calloc(17, sizeof(char));	strcpy(dir_mp4, "svr_database/mp4");
	char* dir_264 = (char*) calloc(17, sizeof(char));	strcpy(dir_264, "svr_database/264");
	DIR *dp;
	struct dirent *dirp;
	if ((dp = opendir(dir_mp4)) == NULL){
	return -1;
	}
	while ((dirp = readdir(dp)) != NULL){
		if(strlen(dirp->d_name) >4){
			char* vid_name = (char*) calloc(30, sizeof(char));
			memset(vid_name, '\0', sizeof(vid_name));
			char* path_mp4 = (char*) calloc(60, sizeof(char));
			char* path_264 = (char*) calloc(60, sizeof(char));
			strncpy(vid_name, dirp->d_name,  strlen(dirp->d_name) -4);
			char* name_264 = (char*) calloc(sizeof(vid_name)+4, sizeof(char));	strcpy(name_264, vid_name);	strcat(name_264,".264");
			strcpy(path_mp4, dir_mp4);	strcat(path_mp4, "/");	strcat(path_mp4, vid_name);	strcat(path_mp4, ".mp4");
			strcpy(path_264, dir_264);	strcat(path_264, "/");	strcat(path_264, vid_name);	strcat(path_264, ".264");
			long size_mp4 = -1, size_264 = -1;
			bool encoded;
			size_mp4 = get_file_size(path_mp4);
			
			if(search_file(dir_264, name_264)){
				encoded = true;
				size_264 = get_file_size(path_264);
			}
			else{
				encoded = false;
			}
			vid_obj_t* vid_obj = (vid_obj_t*) calloc(1, sizeof(vid_obj_t));
			vid_obj->name = vid_name;
			vid_obj->path_mp4 = path_mp4;
			vid_obj->path_264 = path_264;
			vid_obj->size_mp4 = size_mp4;
			vid_obj->size_264 = size_264;
			vid_obj->encoded = encoded;
			ll_add(vid_list, vid_obj);
			fprintf(stderr, "name: %s\n", vid_name);
			fprintf(stderr, "path_mp4: %s\n", path_mp4);
			fprintf(stderr, "path_264: %s\n", path_264);
			fprintf(stderr, "size_mp4: %ld bytes\n", size_mp4);
			fprintf(stderr, "encoded: %d\n", encoded);
			fprintf(stderr, "size_264: %ld bytes\n", size_264);
			fprintf(stderr, "____________________________________\n");
		}
	}
	closedir(dp);
	return 0;
}