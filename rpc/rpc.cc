// C Headers
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//C++ Headers
#include <iostream>
#include <string>
#include <cstring>

// Local headers
#include "rpc.h"
#include "rpc_consts.h"
#include "util.h"

using namespace std;
char * binderAddr;
char * binderPort;
int bindDescriptor;

int connectBinder() {
	struct hostent     *he;
	struct sockaddr_in  server;

	if ((he = gethostbyname(binderAddr)) == NULL) {
		return -1;
	}

	memcpy(&server.sin_addr, he->h_addr_list[0], he->h_length);
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(binderPort));
	bindDescriptor = socket(AF_INET, SOCK_STREAM, 0);

	if (connect(bindDescriptor, (struct sockaddr *)&server, sizeof(server)) != 0) {
		return -1;
	}
	printf("bindDescriptor: %d\n", bindDescriptor);
	return 0;
}
int rpcInit(){
	binderAddr = getenv("BINDER_ADDRESS");
	binderPort = getenv("BINDER_PORT");
	if (binderAddr == NULL) { 
		cerr << "Env variable BINDER_ADDRESS not found" << endl;
		return 1;
	}
	if (binderPort == NULL) { 
		cerr << "Env variable BINDER_PORT not found" << endl;
		return 1;
	}
	if (connectBinder() != 0) {
		printf("Could not connect to binder\n");
		perror("Connect: ");
	}else if(VERBOSE_OUTPUT == 1){
		printf("Connecting to binder succeeded\n");
	}
	//if (setupListener() == ) {}		
	return 0;
}

int rpcCall(char* name, int* argTypes, void** args){
	return 0;
}

int rpcCacheCall(char* name, int* argTypes, void** args){
	return 0;
}

int rpcRegister(char* name, int* argTypes, skeleton f){
	int totalMsgSize = 0;
	totalMsgSize += sizeof(char); 	// RPC call
	totalMsgSize += sizeof(int); 	// Length of funtion name
	totalMsgSize += strlen(name); 	// Name
	totalMsgSize += sizeof(int); 	// Size of argsarray

	int argSize = 0;
	while (argTypes[argSize]) { 
		argSize++;
	}	

	//Arg is not always int though? Dan please check?
	totalMsgSize += argSize * sizeof(int); // Args
	totalMsgSize += sizeof(int); // 0
	totalMsgSize += sizeof(int); // skeleton int*
	totalMsgSize += sizeof(int); // skeleton void**
	char buffer[totalMsgSize*2];

	char call_type = RPC_REGISTER;
	int counter = 0;
	memcpy(buffer+counter, &call_type, sizeof(char));

	counter += sizeof(char);
	int nameLength = strlen(name);

	// Copy in name length
	char int_arr[4];
	intToArr(nameLength, int_arr); 
	memcpy(buffer+counter, int_arr, 4);
	counter += sizeof(4);

	// Copy in name
	memcpy(buffer+counter, name, nameLength);
	counter = counter + nameLength;

	// Copy in arg size
	intToArr(argSize, int_arr); 
	memcpy(buffer+counter, int_arr, 4);
	counter += sizeof(4);

	// Copy in args
	for (int i = 0; i < argSize; i++) { 
		memcpy(buffer+counter, &argTypes[i], sizeof(int));
		counter = counter + sizeof(int);
	}

	// Write loop
	int written = 0, result = 0;
	while(written < totalMsgSize){
		result = write(bindDescriptor, buffer+written, totalMsgSize-written);
		if(result == -1){return result;}
		written += result;
	}
	
	// Return number of bytes written
	return written;
}

int rpcExecute(){
	return 0;
}

int rpcTerminate(){
	return 0;
}
