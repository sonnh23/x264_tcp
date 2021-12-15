#include "handle_file.h"

int send_file(int client_socket, FILE* fp){

	/*get size of file*/
	fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);
	char size_str[20];
	sprintf(size_str, "%ld", file_size);

	/*send size of file to client*/
	send(client_socket, size_str, sizeof(size_str), 0);

	//send binary file
	if (file_size > 0){
        char buffer[SIZE];
		long bytes_left = file_size;
		int count = 0;
        while(bytes_left > 0){
			bzero(buffer, SIZE);
			size_t num;
			if(bytes_left < sizeof(buffer))
				num = bytes_left;
			else
				num = sizeof(buffer);
            num = fread(buffer, 1, num, fp);

            num = send(client_socket, buffer, num, 0);
            bytes_left -= num;
			//fprintf(stderr, "%d\t\t%ld\t\t%ld\t\t\n",count, num, bytes_left);
			fprintf(stderr,"\rSent %ld / %ld bytes\t",file_size - bytes_left, file_size);
			if(bytes_left == 0)
				fprintf(stderr,"Done!\n");
			count++;
        };
    }
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
	char buffer[SIZE];
	recv(socket, buffer, SIZE, 0);
	char* ptr;
	long file_size = strtol(buffer, &ptr, 10);		//get size of binary file.
	if(file_size <= 0){
		return -1;
	}

	fprintf(stderr, "Server:\tFile has size of %ld bytes, start transferring...\n", file_size);
	FILE *fp = fopen(path_264, "wb");
	if(fp == NULL)
		return 0;
	else{
		bzero(buffer,SIZE);
		long bytes_left = file_size;
		size_t len;
		int count =0;
		while (bytes_left > 0){
			bzero(buffer,SIZE);
			if(bytes_left < sizeof(buffer))
				len= bytes_left;
			else
				len = sizeof(buffer);

			len = recv(socket, buffer, len, 0);
			
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