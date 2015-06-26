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
#include <string>

typedef struct{
	std::string name;
	std::string function_data;
	std::string server_ip;
} func_def;

// Maps functions to server locations
std::vector<func_def> function_database;

int fourBytesToInt(char* buffer){
	return 	(((unsigned char)buffer[0]) << 24) +
			(((unsigned char)buffer[1]) << 16) +
			(((unsigned char)buffer[2]) << 8)  +
			 ((unsigned char)buffer[3]);			
}