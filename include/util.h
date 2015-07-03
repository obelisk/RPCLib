#include <stdio.h>
#include <unistd.h>

// Used in binder and rpcCall
typedef struct{
	unsigned char input;
	unsigned char output;
	unsigned char type;
	unsigned int length;
} param_t;

typedef struct{
	std::string server;
	int port;
} server_t;

typedef struct{
	std::string name;
	int param_count;
	param_t* params;
	std::deque<server_t> servers;
} func_def_t;

void intToArr(int value, char out[4]){
	unsigned int mask = 0xFF000000;
	out[0] = ((value & mask) >> 24);
	mask = mask >> 8;
	out[1] = ((value & mask) >> 16);
	mask = mask >> 8;
	out[2] = ((value & mask) >> 8);
	mask = mask >> 8;
	out[3] = (value & mask);
}

void arrToInt(int *out, char in[4]){
	*out = 	(((unsigned char)in[0]) << 24) +
			(((unsigned char)in[1]) << 16) +
			(((unsigned char)in[2]) << 8)  +
			 ((unsigned char)in[3]);
}

void englishType(int type){
	switch(type){
		case ARG_CHAR:
			printf("Char");
			break;
		case ARG_SHORT:
			printf("Short");
			break;
		case ARG_INT:
			printf("Int");
			break;
		case ARG_LONG:
			printf("Long");
			break;
		case ARG_DOUBLE:
			printf("Double");
			break;
		case ARG_FLOAT:
			printf("Float");
			break;
		default:
			printf("Unknown");
			break;
	}
}

int readNBytes(int des, int amount, char* buffer){
	int so_far = 0, result = 0;
	while(so_far != amount){
		result = read(des, buffer+so_far, 1);
		if(result <= 0){
			return -1;
		}else{
			so_far += result;
		}
	}
	return so_far;
}