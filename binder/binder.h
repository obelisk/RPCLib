// Standard C Headers
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/socket.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// Stardard C++ Headers
#include <vector>
#include <deque>
#include <string>

// Local Headers
#include "util.h"

// Maps functions to server locations
std::vector<func_def_t> function_database;

int fourBytesToInt(char* buffer){
	return 	(((unsigned char)buffer[0]) << 24) +
			(((unsigned char)buffer[1]) << 16) +
			(((unsigned char)buffer[2]) << 8)  +
			 ((unsigned char)buffer[3]);			
}