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

#define NEW_SERVER 		0x1
#define NEW_FUNCTION 	0x2
#define RE_REGISTER		0x3
#define NO_FUNCTION		0x4

typedef struct {
	std::vector<func_def_t> supported_functions;
	std::string server;
	int port;
} server_data_t;

// Server lookup list
std::deque<server_data_t> server_list;