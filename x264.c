#include "x264.h"

int encode_264(char* vid_name){
	char encode[4096];
	memset(encode, 0, 4096);
	/*decode*/
	sprintf(encode, "./x264/x264 -o svr_database/264/%s.264 svr_database/mp4/%s.mp4", vid_name, vid_name);
	return system(encode);
}
int decode_264(char* vid_name){
	//decode
	char decode[4096];
	sprintf(decode,"./x264/x264 --input-res 1920x1080 -o clt_database/mkv/%s.mkv clt_database/264/%s.264", vid_name, vid_name);
	return system(decode);
}