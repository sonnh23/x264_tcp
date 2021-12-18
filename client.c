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

const int msg_len = 65536;
enum Mode {Terminal, Gui};
int main(int argc, char *argv[]){
	enum Mode mode;

	if(argc != 4 || (strcmp(argv[3], "-t") != 0 && strcmp(argv[3], "-g") != 0)){
		fprintf(stderr, "Usage: \t./client <server ip> <server port> <mode>\n");
		fprintf(stderr, "\t  mode:\t-t: (Terminal mode)\n\t\t-g: (GUI mode)\n");
		exit(0);
	}

	char* server_ip = argv[1]; 
	int server_port = atoi(argv[2]);

	if(strcmp(argv[3], "-t") == 0){
		mode = Terminal;
		fprintf(stderr, "[Terminal mode]\n");
	}
	else if(strcmp(argv[3], "-g") == 0){
		mode = Gui;
		fprintf(stderr, "[GUI mode]\n");
	}

	int client_socket, ret;
	struct sockaddr_in server_addr;
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(client_socket < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("Client socket is created.\n");

	memset(&server_addr, '\0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(server_port);
	server_addr.sin_addr.s_addr = inet_addr(server_ip);

	ret = connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr));
	if(ret < 0){
		printf("Error in connection.\n");
		exit(1);
	}
	printf("\n[+] Connected to sserver %s: %d\n", server_ip, server_port);
	
	while(1){
		char* vid_name = (char*) calloc(30,sizeof(char));
		if(mode ==  Gui){
			FILE *pytalk = fopen("com/pytalk.txt", "r");
			if(pytalk == NULL){
				fprintf(stderr, "\rCan not read pytalk.txt ");
				exit(0);
			}
			/* get the length of file content*/
			fseek(pytalk, 0, SEEK_END);
			long file_len = ftell(pytalk);
			rewind(pytalk);
			//fprintf(stderr, "\rLen is %ld", file_len);
			if(file_len <= 0){
				fclose(pytalk);
			}
			else if(file_len > 0){
				/*if video name was written to pytalk, read it then send to server and clear the file for next loop*/
				fgets(vid_name, sizeof(vid_name), pytalk);
				fprintf(stderr, "%s\n", vid_name);
				if(strcmp(vid_name, ":q") == 0){
					send(client_socket, vid_name, strlen(vid_name), 0);
					close(client_socket);
					printf("[-] Disconnected from server.\n");
					exit(1);
				}
				send(client_socket, vid_name, strlen(vid_name), 0);
				fprintf(stderr, "\t=>Send requesting for '%s.mp4', wating for server response...\n", vid_name);
				fclose(pytalk);
				FILE* fp = fopen("com/pytalk.txt", "w");
				fclose(fp);
		
				/* receiving process*/
				char *path_264 = (char*) calloc(sizeof(vid_name)+22, sizeof(char));
				sprintf(path_264,"clt_database/264/%s.264",vid_name);
				int val = recv_file(client_socket, path_264);
				free(path_264);
			
				int ctalk_val;		//ctalk_val is value that would be written to ctalk to tell client.py about server response
				if (val == -1){
					fprintf(stderr, "[!] Server:\tFile does not exist\n");
					ctalk_val -1;
				}
				else if(val == 0){
					fprintf(stderr, "Error in reciving file\n");
					ctalk_val = 0;
				}else if(val == 1){
					fprintf(stderr, "Done!\n");
					fprintf(stderr, "Start decoding...\n");
					if(!decode_264(vid_name)){
						ctalk_val = 1;
						fprintf(stderr, "'%s.264' was decoded to '%s.mkv'\n", vid_name, vid_name);
					}else{
						ctalk_val = 0;
						fprintf(stderr, "Error in decoding\n");
					}
				}
				/*wtrite ctalk_val to ctalk.txt then client.py would read it*/
				//fprintf(stderr, "ctalk_val is %d\n", ctalk_val);
				FILE* ctalk = fopen("com/ctalk.txt", "w");
				fprintf(ctalk, "%d", ctalk_val);
				fclose(ctalk);
			}
		}
		else if(mode == Terminal){
			/*if video name was written to pytalk, read it then send to server and clear the file for next loop*/
			fprintf(stderr, "Enter video name:\t");
			fflush(stdin);
			gets(vid_name);
			if(strcmp(vid_name, ":q") == 0){
				send(client_socket, vid_name, strlen(vid_name), 0);
				close(client_socket);
				printf("[-] Disconnected from server.\n");
				exit(1);
			}
			send(client_socket, vid_name, strlen(vid_name), 0);
			fprintf(stderr, "\t=>Send requesting for '%s.mp4', wating for server response...\n", vid_name);
			/* receiving process*/
			char *path_264 = (char*) calloc(sizeof(vid_name)+22, sizeof(char));
			sprintf(path_264,"clt_database/264/%s.264",vid_name);
			int val = recv_file(client_socket, path_264);
			free(path_264);
		
			int ctalk_val;		//ctalk_val is value that would be written to ctalk to tell client.py about server response
			if (val == -1){
				fprintf(stderr, "[!] Server:\tFile does not exist\n");
				ctalk_val -1;
			}
			else if(val == 0){
				fprintf(stderr, "Error in reciving file\n");
				ctalk_val = 0;
			}else if(val == 1){
				fprintf(stderr, "Done!\n");
				fprintf(stderr, "Start decoding...\n");
				if(!decode_264(vid_name)){
					ctalk_val = 1;
					fprintf(stderr, "'%s.264' was decoded to '%s.mkv'\n", vid_name, vid_name);
				}else{
					ctalk_val = 0;
					fprintf(stderr, "Error in decoding\n");
				}
				
				//play
				char play[2048];
				sprintf(play," xdg-open clt_database/mkv/%s.mkv", vid_name);
				system(play);
			}
		}
	free(vid_name);
	}
	return 0;
}