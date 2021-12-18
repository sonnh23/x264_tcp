#include "handle_file.h"

long get_file_size(char* file_path){
    FILE *fp = fopen(file_path, "r");
    if(fp == NULL)
        return -1;
    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);
    fclose(fp);
    return file_size;
}
/*
int send_file(int client_socket, FILE* fp){

	//get size of file
	fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);
	char size_str[20];
	sprintf(size_str, "%ld", file_size);

	//send size of file to client
	send(client_socket, size_str, sizeof(size_str), 0);

	//send binary file
	if (file_size > 0){
        char buffer[msg_len];
		long bytes_left = file_size;
		int count = 0;
        while(bytes_left > 0){
			bzero(buffer, msg_len);
			size_t num;
			if(bytes_left < sizeof(buffer))
				num = bytes_left;
			else
				num = sizeof(buffer);
            num = fread(buffer, 1, num, fp);
            send_msg(client_socket, buffer, num);
            bytes_left -= num;
			//fprintf(stderr, "%d\t\t%ld\t\t%ld\t\t\n",count, num, bytes_left);
			fprintf(stderr,"\rSent %ld / %ld bytes\t",file_size - bytes_left, file_size);
			if(bytes_left == 0)
				fprintf(stderr,"Done!\n");
			count++;
        };
    }
}*/
int send_vid(int client_socket, vid_obj_t* vid_obj){
	long file_size = vid_obj->size_264;
	char size_264[20];
	sprintf(size_264, "%ld", file_size);
	fprintf(stderr, "size of 264 file: %ld\n", file_size);
	send(client_socket, size_264, sizeof(size_264), 0);
	FILE *fp = fopen(vid_obj->path_264, "r");
	if (file_size > 0){
        char buffer[msg_len];
		long bytes_left = file_size;
		int count = 0;
        while(bytes_left > 0){
			bzero(buffer, msg_len);
			size_t num;
			if(bytes_left < sizeof(buffer))
				num = bytes_left;
			else
				num = sizeof(buffer);
            num = fread(buffer, 1, num, fp);
            send_msg(client_socket, buffer, num);
            bytes_left -= num;
			//fprintf(stderr, "%d\t\t%ld\t\t%ld\t\t\n",count, num, bytes_left);
			fprintf(stderr,"\rSent %ld / %ld bytes\t",file_size - bytes_left, file_size);
			if(bytes_left == 0)
				fprintf(stderr,"Done!\n");
			count++;
        };
    }
	fclose(fp);
}
int search_file(char *dir_name, char* file_name){
	DIR *dp;
	struct dirent *dirp;
	if ((dp = opendir(dir_name)) == NULL){
	return -1;
	}
	while ((dirp = readdir(dp)) != NULL){
		if(!strcmp(dirp->d_name,file_name)){
			closedir(dp);
			return 1;
		}
	}
	closedir(dp);
	return 0;
}
int recv_file(int socket, char* path_264){
	char buffer[msg_len];
	recv(socket, buffer, msg_len, 0);
	char* ptr;
	long file_size = strtol(buffer, &ptr, 10);		//get size of binary file.
	if(file_size <= 0){
		return -1;
	}

	fprintf(stderr, "[!] Server:\tFile encoded has size of %ld bytes, start transferring...\n", file_size);
	FILE *fp = fopen(path_264, "wb");
	if(fp == NULL)
		return 0;
	else{
		bzero(buffer,msg_len);
		long bytes_left = file_size;
		size_t len;
		int count =0;
		while (bytes_left > 0){
			bzero(buffer,msg_len);
			if(bytes_left < sizeof(buffer))
				len= bytes_left;
			else
				len = sizeof(buffer);

			recv_msg(socket, buffer, len);
			
			bytes_left -= len;
			//fprintf(stderr, "%d\t\t%ld\t\t%ld\t\t\n",count, len, bytes_left);
			fwrite(buffer, 1, len, fp);
			fprintf(stderr, "\rReceived %ld / %ld bytes\t", file_size - bytes_left, file_size);
			count++;
			if(bytes_left == 0 || len == 0){
				return 1;
			}
		};
	}
	fclose(fp);
}
void recv_msg(int socket, char* buf, int buf_len){
    while (buf_len > 0){
        int num = recv(socket, buf, buf_len, 0);
        //buf += num;
        buf_len -= num;
    }
}
void send_msg(int socket, char* buf, int buf_len){
    while (buf_len > 0){
        int num = send(socket, buf, buf_len, 0);
        //buf += num;
        buf_len -= num;
    }
}