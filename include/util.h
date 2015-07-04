// Standard C Headers
#include <stdio.h>
#include <unistd.h>

// Standard C++ Headers
#include <deque>
#include <string>

// Local Headers
#include "rpc.h"

// Used in binder and rpcCall
typedef struct {
	unsigned char input;
	unsigned char output;
	unsigned char type;
	unsigned int length;
} param_t;

typedef struct {
	std::string server;
	int port;
} server_t;

typedef struct {
	std::string name;
	int param_count;
	param_t *params;
	std::deque<server_t> servers;
} func_def_t;

void intToArr(int value, char out[4]);
void arrToInt(int *out, char in[4]);
void englishType(int type);
int readNBytes(int des, int amount, char *buffer);