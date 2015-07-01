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
int serverDescriptor;

int setupListener() {
        int s = socket(AF_INET,SOCK_STREAM, 0);
        struct sockaddr_in sAddr;
        sAddr.sin_family = AF_INET;
        sAddr.sin_addr.s_addr = INADDR_ANY;
        sAddr.sin_port = 0;
        bind (s, (struct sockaddr *)&sAddr, sizeof(sAddr));
        return s;
}

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
int clientInit() { 
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
	serverDescriptor = setupListener();
	return 0;
}

int rpcCall(char* name, int* argTypes, void** args){
	clientInit();
	int binderMsgSize = 0; 
	binderMsgSize += sizeof(char);
	binderMsgSize += sizeof(int);
	binderMsgSize += strlen(name);
	binderMsgSize += sizeof(int);
	int argSize = 0;

	while (argTypes[argSize]) { 
		argSize++;
	}
	binderMsgSize += argSize * sizeof(int);

	char buffer[binderMsgSize];
	// writing rpc call
	char call_type = RPC_CALL;
	int counter = 0;
	memcpy(buffer+counter, &call_type, sizeof(char));
	counter += sizeof(char);

	// writting namelength
	int nameLength = strlen(name);
	char int_arr[4];
	intToArr(nameLength, int_arr);
	memcpy(buffer+counter, int_arr, 4);
	counter += sizeof(4);

	// writting name
	memcpy(buffer+counter, name, nameLength);
	counter += nameLength;

	// writting arg size
	intToArr(argSize, int_arr);
	memcpy(buffer+counter, int_arr, 4);
	counter += sizeof(4);

	// writing args
	for (int i = 0; i < argSize; i++) { 
		intToArr(argTypes[i], int_arr);
		memcpy(buffer+counter, int_arr, 4);
		counter += sizeof(4);
	}
	
	int written = 0, result = 0;
	while (written < binderMsgSize) { 
		result = write(bindDescriptor, buffer+written, binderMsgSize-written);
		if (result == -1) { 
			return result;
		}
		written += result;
	}
	return 0;
}

int rpcCacheCall(char* name, int* argTypes, void** args){
	return 0;
}

int rpcRegister(char* name, int* argTypes, skeleton f){
	int totalMsgSize = 0;
	totalMsgSize += sizeof(char); 	// RPC call
	totalMsgSize += sizeof(int);    // server port;
	totalMsgSize += sizeof(int); 	// Length of funtion name
	totalMsgSize += strlen(name); 	// Name
	totalMsgSize += sizeof(int); 	// Size of argsarray

	int argSize = 0;
	while (argTypes[argSize]) { 
		argSize++;
	}	

	//The size of int thing is nice, but everything
	//will break if it's ever not 4. This is because
	//of both client and server side issues. Wishlist

	totalMsgSize += argSize * sizeof(int); // Args
	char buffer[totalMsgSize];

	char call_type = RPC_REGISTER;
	int counter = 0;
	memcpy(buffer+counter, &call_type, sizeof(char));

	counter += sizeof(char);
	int nameLength = strlen(name);
	
	// copying port
 	char int_arr[4];
	struct sockaddr_in sin;
    socklen_t addrlen = sizeof(sin);
    getsockname(serverDescriptor, (struct sockaddr *)&sin, &addrlen);
	intToArr(ntohs(sin.sin_port), int_arr);
    memcpy(buffer+counter, int_arr, sizeof(int));
	counter += sizeof(int);

	// Copy in name length
	intToArr(nameLength, int_arr); 
	memcpy(buffer+counter, int_arr, 4);
	counter += sizeof(int);

	// Copy in name
	memcpy(buffer+counter, name, nameLength);
	counter += nameLength;

	// Copy in arg size
	intToArr(argSize, int_arr); 
	memcpy(buffer+counter, int_arr, 4);
	counter += sizeof(int);

	// Copy in args
	for (int i = 0; i < argSize; i++) {
		intToArr(argTypes[i], int_arr); 
		memcpy(buffer+counter, int_arr, 4);
		counter += sizeof(int);
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
