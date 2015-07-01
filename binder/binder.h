#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <vector>
#include <deque>
#include <string>

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


// Maps functions to server locations
std::vector<func_def_t> function_database;

int fourBytesToInt(char* buffer){
	return 	(((unsigned char)buffer[0]) << 24) +
			(((unsigned char)buffer[1]) << 16) +
			(((unsigned char)buffer[2]) << 8)  +
			 ((unsigned char)buffer[3]);			
}