#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "handle_file.h"
#include "x264.h"
const int msg_len = 65536;
int handle_dtb(int new_socket, char* file_name);
int main(int argc, char *argv[]){
	/*if(argc != 3){
		fprintf(stderr, "Usage: \t./server <server ip> <server port>\n");
		exit(0);
	}*/
	char* server_ip = "127.0.0.1"; //argv[1];
	int server_port = 8080; //atoi(argv[2]);
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
					}
				}
			}
		}
	}
	return 0;
}
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
