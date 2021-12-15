#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "handle_file.h"
#include "x264.h"
#define PORT 8080
#define SIZE 65535

int decode_264(char* vid_name){
	//decode
	char decode[4096];
	sprintf(decode,"./x264/x264 --input-res 1920x1080 -o clt_database/mkv/%s.mkv clt_database/264/%s.264", vid_name, vid_name);
	return system(decode);
}
int main(){
	int client_socket, ret;
	struct sockaddr_in server_addr;
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(client_socket < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Client Socket is created.\n");

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ret = connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(ret < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Connected to Server.\n");
	
	while(1){
		char vid_name[25];
		printf("Client:\t");
		fflush(stdin);
		gets(vid_name);
		if(strcmp(vid_name, ":q") == 0){
			close(client_socket);
			printf("Disconnected from server.\n");
			exit(1);
		}
		send(client_socket, vid_name, strlen(vid_name), 0);

		char *path_264 = (char*) calloc(sizeof(vid_name)+22, sizeof(char));
		sprintf(path_264,"clt_database/264/%s.264",vid_name);
		
		int val = recv_file(client_socket, path_264);
		free(path_264);
		if (val == -1){
			fprintf(stderr, "Server:\tFile does not exist\n");
		}
		else if(val == 0){
			fprintf(stderr, "Error in reciving file\n");
		}else if(val == 1){
			fprintf(stderr, "Done!\n");
			fprintf(stderr, "Start decoding...\n");
			if(!decode_264(vid_name))
				fprintf(stderr, "'%s.264' was decoded to '%s.mkv'\n", vid_name, vid_name);
			else
				fprintf(stderr, "Error in decoding\n");

			//play
			char play[2048];
			sprintf(play," xdg-open clt_database/mkv/%s.mkv", &vid_name);
			system(play);
		}
	}
	return 0;
}

